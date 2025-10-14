#include "ots.h"
#include "common.h"

/*
uint16_t ots_feature_chr_handle;
uint16_t object_name_chr_handle;
uint16_t object_type_chr_handle;
uint16_t object_size_chr_handle;
uint16_t object_first_created_chr_handle;
uint16_t object_last_modified_chr_handle;
uint16_t object_id_chr_handle;
uint16_t object_properties_chr_handle;
uint16_t object_action_control_point_chr_handle;
uint16_t object_list_control_point_chr_handle;
uint16_t object_list_filter_chr_handle;
uint16_t object_changed_chr_handle;
*/

SVC_DEFINE_UUID16(ots, 0x1825);

CHR_DEFINE_UUID16(ots_feature_chr_uuid, 0x2ABD);
CHR_DEFINE_UUID16(object_name_chr_uuid, 0x2ABE);
CHR_DEFINE_UUID16(object_type_chr_uuid, 0x2ABF);
CHR_DEFINE_UUID16(object_size_chr_uuid, 0x2AC0);
CHR_DEFINE_UUID16(object_first_created_chr_uuid, 0x2AC1);
CHR_DEFINE_UUID16(object_last_modified_chr_uuid, 0x2AC2);
CHR_DEFINE_UUID16(object_id_chr_uuid, 0x2AC3);
CHR_DEFINE_UUID16(object_properties_chr_uuid, 0x2AC4);
CHR_DEFINE_UUID16(object_action_control_point_chr_uuid, 0x2AC5);
CHR_DEFINE_UUID16(object_list_control_point_chr_uuid, 0x2AC6);
CHR_DEFINE_UUID16(object_list_filter_chr_uuid, 0x2AC7);
CHR_DEFINE_UUID16(object_changed_chr_uuid, 0x2AC8);
