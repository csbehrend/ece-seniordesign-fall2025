/*
 * ============================================================================
 * Filename   : ots_store.c
 * Author     : Connor Behrend
 * Description:
 * ============================================================================
 */

#include "ots_store.h"
#include "coc.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "host/ble_hs.h"
#include "host/ble_l2cap.h"
#include "host/ble_uuid.h"
#include "nimble/nimble_npl_os.h"
#include "os/os_mbuf.h"
#include "ots.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint32_t m_testval = 4567;
static uint8_t m_array[4096] = {0};

static oacp_read_cb generic_mem_read;
static oacp_write_cb generic_mem_write;

static const TickType_t wait_ticks = pdTICKS_TO_MS(100);

static const char testval_name[] = "TestVal";
static const char bigarr_name[] = "BigArr";

static ots_object_t test[] = {
    {
        .name = testval_name,
        .name_len = sizeof(testval_name),
        .type =
            BLE_UUID128_DECLARE(0xc1, 0x8c, 0xe9, 0x55, 0x0c, 0xf6, 0xec, 0x8a,
                                0x2d, 0x44, 0xf3, 0x30, 0x8d, 0x70, 0x5d, 0xc1),
        .size.decoded = {.current_size = sizeof(m_testval),
                         .allocated_size = sizeof(m_testval)},
        .id.luid = {0xce, 0x70, 0xe2, 0xf8, 0x8d, 0xc1},
        .properties.decoded = {.read = 1, .write = 1, .truncate = 1},
        .cb_func = {.read = generic_mem_read, .write = generic_mem_write},
        .base = &m_testval,
    },
    {
        .name = bigarr_name,
        .name_len = sizeof(bigarr_name),
        .type =
            BLE_UUID128_DECLARE(0xab, 0x77, 0xb7, 0xdb, 0x9f, 0x65, 0x4d, 0x9c,
                                0x7f, 0x4b, 0xc5, 0xf4, 0xfb, 0x51, 0xe7, 0x3b),
        .size.decoded = {.current_size = sizeof(m_array),
                         .allocated_size = sizeof(m_array)},
        .id.luid = {0x9e, 0x98, 0x42, 0x13, 0x78},
        .properties.decoded = {.read = 1, .write = 1},
        .cb_func = {.read = generic_mem_read},
        .base = m_array,
    }};

ots_object_t *g_obj_array = test;
size_t g_obj_array_len = ARRAY_SIZE(test);
size_t g_obj_current = 0;
SemaphoreHandle_t g_ots_mutex = NULL;

static int generic_mem_read(ots_object_t *obj, struct os_mbuf *buf,
                            uint32_t offset, uint32_t len) {
  if (!obj || !buf || offset + len > obj->size.decoded.current_size) {
    return -1;
  }
  assert(obj->base != NULL);
  return os_mbuf_append(buf, obj->base + offset, len);
}

static int generic_mem_write(ots_object_t *obj, struct os_mbuf *buf,
                             uint32_t offset, uint32_t len) {
  if (!obj || !buf || !(len > 0) ||
      offset + len > obj->size.decoded.allocated_size) {
    return -1;
  }
  assert(obj->base != NULL);
  assert(buf->om_len == len);

  memcpy(obj->base + offset, buf->om_data, len);
  return 0;
}

int lockOtsTable() {
  assert(g_ots_mutex != NULL);
  return xSemaphoreTake(g_ots_mutex, wait_ticks);
}
void unlockOtsTable() {
  assert(g_ots_mutex != NULL);
  xSemaphoreGive(g_ots_mutex);
}

static int oacp_indicate(uint16_t conn_handle, oacp_response_t *response) {
  int rc = 0;
  assert(response != NULL);
  struct os_mbuf *txom = ble_hs_mbuf_from_flat(response, sizeof(*response));
  if (!txom) {
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  rc = ble_gatts_indicate_custom(conn_handle,
                                 object_action_control_point_chr_handle, txom);
  if (response->result == OACP_RESULT_SUCCESS) {
    ESP_LOGI(TAG, "oacp_indicate: success, start operation");
  } else {
    ESP_LOGE(TAG, "oacp_indicate: error id %d, cancel operation",
             response->result);
  }
  return rc;
}

void ots_store_init() {
  g_ots_mutex = xSemaphoreCreateBinary();
  if (g_ots_mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create mutex for OTS object table");
    assert(0);
    return;
  }
  for (int i = 0; i < ARRAY_SIZE(m_array); ++i) {
    m_array[i] = i;
  }
  unlockOtsTable();
}

static int oacp_read_operation(uint16_t conn_handle, ots_object_t *object,
                               uint32_t offset, uint32_t length) {
  assert(object != NULL);
  ESP_LOGI(TAG, "Service read operation for object \"%s\" off(%d) len(%d)",
           object->name, offset, length);
  int rc = 0;
  oacp_response_t response = {.op = OACP_OP_READ,
                              .result = OACP_RESULT_SUCCESS};

  struct ble_l2cap_chan *chan = coc_acquire_channel();
  if (chan == NULL) {
    response.result = OACP_RESULT_CHANNEL_UNAVAILBLE;
    return oacp_indicate(conn_handle, &response);
  }
  rc = oacp_indicate(conn_handle, &response);
  assert(rc == 0);

  struct os_mbuf *data = alloc_coc_mbuf();
  assert(data != NULL);

  if (object->cb_func.read(object, data, offset, length)) {
    os_mbuf_free_chain(data);
    goto error;
  }

  rc = ble_l2cap_send(chan, data);

  struct ble_l2cap_event event = {0};
  while (rc == BLE_HS_ESTALLED) {
    rc = await_coc_event(&event);
    if (rc != pdPASS) {
      goto error;
    }
    if (event.type != BLE_L2CAP_EVENT_COC_TX_UNSTALLED) {
      rc = 1;
      goto error;
    }
    rc = ble_l2cap_send(chan, data);
  }

  /*
  rc = await_coc_event(&event);
  if (rc != BLE_L2CAP_EVENT_COC_DISCONNECTED) {
    ESP_LOGE(TAG, "client did not initiate disconnect");
    goto error;
  }
  */

  coc_release_channel();
  ESP_LOGI(TAG, "SUCCESSFULLY serviced read operation for object %s",
           object->name);
  return 0;

error:
  coc_force_close();
  ESP_LOGE(TAG, "FAILED to service read operation for object %s", object->name);
  return rc;
}

static int oacp_write_operation(uint16_t conn_handle, ots_object_t *object,
                                uint32_t offset, uint32_t length) {
  assert(object != NULL);
  assert(offset == 0 && length > 0);
  ESP_LOGI(TAG, "Service WRITE operation for object \"%s\" off(%d) len(%d)",
           object->name, offset, length);
  int rc = 0;
  oacp_response_t response = {.op = OACP_OP_WRITE,
                              .result = OACP_RESULT_SUCCESS};

  struct ble_l2cap_chan *chan = coc_acquire_channel();
  if (chan == NULL) {
    response.result = OACP_RESULT_CHANNEL_UNAVAILBLE;
    return oacp_indicate(conn_handle, &response);
  }
  rc = oacp_indicate(conn_handle, &response);
  assert(rc == 0);

  // Clear object
  object->size.decoded.current_size = 0;
  oacp_write_cb *write = object->cb_func.write;
  assert(write != NULL);

  uint32_t current_size = 0;
  struct ble_l2cap_event event = {0};
  while (current_size < length) {
    uint32_t remaining_len = length - current_size;
    rc = await_coc_event(&event);
    if (rc != pdPASS) {
      ESP_LOGE(TAG, "coc_event wait error %d", rc);
      goto error;
    }
    if (event.type != BLE_L2CAP_EVENT_COC_DATA_RECEIVED) {
      ESP_LOGE(TAG, "event type not DATA_RECEIVED, instead %d", event.type);
      goto error;
    }
    if (event.receive.sdu_rx->om_len > remaining_len) {
      ESP_LOGE(TAG, "bad packet length %d for remaining_len %d",
               event.receive.sdu_rx->om_len, remaining_len);
      os_mbuf_free(event.receive.sdu_rx);
      goto error;
    }
    struct os_mbuf *buf = event.receive.sdu_rx;
    if (buf->om_len < remaining_len) {
      rc = bleprph_l2cap_coc_accept(event.receive.conn_handle, peer_sdu_size,
                                    chan);
      if (rc) {
        ESP_LOGE(TAG, "coc_accept error %d", rc);
        goto error;
      }
    }
    rc = write(object, buf, current_size, buf->om_len);
    if (rc) {
      ESP_LOGE(TAG, "write callback error");
      goto error;
    }
    current_size += buf->om_len;
    os_mbuf_free(buf);
  }

  rc = await_coc_event(&event);
  if (event.type != BLE_L2CAP_EVENT_COC_DISCONNECTED) {
    ESP_LOGE(TAG, "final event not DISCONNECTED, instead %d", event.type);
    goto error;
  }

  object->size.decoded.current_size = offset + length;
  coc_release_channel();
  ESP_LOGI(TAG, "SUCCESSFULLY serviced WRITE operation for object %s",
           object->name);
  return 0;

error:
  if (event.type == BLE_L2CAP_EVENT_COC_DISCONNECTED) {
    coc_release_channel();
  } else {
    coc_force_close();
  }
  ESP_LOGE(TAG, "FAILED to service WRITE operation for object %s",
           object->name);
  return rc;
}

int oacp_request_worker(uint16_t conn_handle, ots_object_t *object,
                        oacp_request_t *request) {
  assert(object != NULL);
  assert(request != NULL);

  int rc = 0;
  oacp_response_t response = {.op = request->op};

  // Locked by bluetooth task
  /*
  if (lockOtsTable() != pdTRUE) {
    response.result = OACP_RESULT_OBJ_LOCKED;
    ESP_LOGE(TAG, "oacp_request_worker: OTS object locked");
    return oacp_indicate(conn_handle, &response);
  }
  */

  uint32_t offset = 0;
  uint32_t length = 0;

  // TODO: add length and object property check (currently ignored)
  switch (request->op) {
  case OACP_OP_READ:
    offset = request->parameter.checksum_read_param.offset;
    length = request->parameter.checksum_read_param.length;
    if (!object->properties.decoded.read || object->cb_func.read == NULL) {
      response.result = OACP_RESULT_PROCEDURE_NOT_PERMITTED;
      return oacp_indicate(conn_handle, &response);
    }
    if (offset + length > object->size.decoded.current_size || !(length > 0)) {
      response.result = OACP_RESULT_INV_PARAM;
      return oacp_indicate(conn_handle, &response);
    }
    return oacp_read_operation(conn_handle, object, offset, length);
  case OACP_OP_WRITE:
    offset = request->parameter.write_param.offset;
    length = request->parameter.write_param.length;
    // Base case, enforce full overwrite for now
    if (!object->properties.decoded.write ||
        !object->properties.decoded.truncate) {
      response.result = OACP_RESULT_PROCEDURE_NOT_PERMITTED;
      return oacp_indicate(conn_handle, &response);
    }
    if (offset != 0 || length > object->size.decoded.allocated_size ||
        !(length > 0)) {
      response.result = OACP_RESULT_INV_PARAM;
      return oacp_indicate(conn_handle, &response);
    }
    return oacp_write_operation(conn_handle, object, offset, length);
  default:
    response.result = OACP_RESULT_UNSUPP_OP;
    ESP_LOGE(TAG,
             "object_action_control_point_chr_access: invalid OACP opcode %d "
             "past support check ",
             request->op);
    return oacp_indicate(conn_handle, &response);
  }

  return rc;
}
