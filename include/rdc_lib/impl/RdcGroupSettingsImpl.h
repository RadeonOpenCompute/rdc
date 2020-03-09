/*
Copyright (c) 2020 - present Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef RDC_LIB_IMPL_RDCGROUPSETTINGSIMPL_H_
#define RDC_LIB_IMPL_RDCGROUPSETTINGSIMPL_H_


#include <memory>
#include <map>
#include <mutex>
#include <string>
#include "rdc_lib/RdcGroupSettings.h"

namespace amd {
namespace rdc {

class RdcGroupSettingsImpl: public RdcGroupSettings {
 public:
      rdc_status_t rdc_group_gpu_create(rdc_group_type_t type,
        const char* group_name, rdc_gpu_group_t* p_rdc_group_id) override;
      rdc_status_t rdc_group_gpu_destroy(
        rdc_gpu_group_t p_rdc_group_id) override;
      rdc_status_t rdc_group_gpu_add(rdc_gpu_group_t groupId,
        uint32_t gpu_index) override;
      rdc_status_t rdc_group_gpu_get_info(rdc_gpu_group_t p_rdc_group_id,
        rdc_group_info_t* p_rdc_group_info)  override;

      rdc_status_t rdc_group_field_create(uint32_t num_field_ids,
        uint32_t* field_ids, const char* field_group_name,
        rdc_field_grp_t* rdc_field_group_id) override;
      rdc_status_t rdc_group_field_destroy(
        rdc_field_grp_t rdc_field_group_id) override;
      rdc_status_t rdc_group_field_get_info(
        rdc_field_grp_t rdc_field_group_id,
        rdc_field_group_info_t* field_group_info) override;

      RdcGroupSettingsImpl();

 private:
    std::map<rdc_gpu_group_t, rdc_group_info_t> gpu_group_;
    std::map<rdc_field_grp_t, rdc_field_group_info_t> field_group_;
    uint32_t cur_group_id_ = 0;
    uint32_t cur_filed_group_id_ = 0;
    std::mutex group_mutex_;
    std::mutex field_group_mutex_;
};

}  // namespace rdc
}  // namespace amd


#endif  // RDC_LIB_IMPL_RDCGROUPSETTINGSIMPL_H_