#pragma STRING(far)

#include <string.h>
#include <stdio.h>

#include <local/config.h>
#include <local/flash_api.h>

#include <salutil.h>
#include <salfwug.h>

#include <web/web_api.h>
#include "web_http.h"

uint8 const far htm_httpupg[] = "\
<html><head><title>HTTP Upgrade</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0}\
fieldset{width:400px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
input[type=file]{border:1px solid #ddd;padding:8px;width:100%;margin:12px 0;border-radius:3px;background:white}\
input[type=submit]{background:#1e88e5;color:#fff;border:0;padding:8px 16px;cursor:pointer;border-radius:3px;font-weight:bold}\
.note{font-size:12px;color:#777;margin-top:15px;background:#f5f5f5;padding:8px;border-left:3px solid #f44336;border-radius:2px}\
.label{margin-bottom:5px;color:#444;font-weight:bold}\
</style>\
<script>function CfmFwUpg(s){if(confirm('It must reboot your device for the upgrade to take effect, continue?')){document.getElementById('upg-btn').value='Uploading...';document.getElementById('upg-btn').disabled=true;return true}else{return false}}</script></head>\
<body>\
<fieldset>\
<legend>Firmware Upgrade</legend>\
<form method=post enctype=multipart/form-data action=/httpupg.cgi?cmd=fw_upgrade onSubmit=\"return CfmFwUpg(this.form)\">\
<div class=\"label\">Select firmware file to upload:</div>\
<input type=file name=file accept=.bin size=60 maxlength=128>\
<div style=\"margin-top:15px\">\
<input type=submit id=upg-btn name=Upload value=\"Upgrade Firmware\">\
</div>\
<div class=\"note\">Warning: Do not power off the device during firmware upgrade.</div>\
</form>\
</fieldset>\
</body></html>\n\
";

static http_image_stat_t g_firm_update_stat = HTTP_IMAGE_IDLE;

int32 get_httpupg(http_request_t *pReq)
{
    uint8 *pValue = NULL;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_httpupg, sizeof(htm_httpupg));

    WEB_END(pReq);

    return SUCCESS;
}

int32 post_http_firmupdate(http_request_t *pReq)
{
    int32 memcpy_len;

#if 0
	static uint32 cnt;
printf("LINE :%d, Packet %ld. pReq->upload_data = %p, len = %ld, offset = %ld, size = %ld\n", \
    (uint16)__LINE__, ++cnt, pReq->upload_data, pReq->upload_len, pReq->upload_offset, pReq->upload_size);
printf("Dump First 16 Data:\n");
#endif

#if 0    
    for (i = 0; i < 16; i++)
        printf("0x%02bx, ", pReq->upload_data[i]);
    printf("\n");
#endif

    /* write data into flash */
    if (0 == pReq->upload_offset)
    {
        /* first packet: receive the packet and change state to tranfering */
        g_firm_update_stat = HTTP_IMAGE_TRANSFERING;

        // upgrade_init(FWUG_UNKNOWN_BIN_SIZE);
        // upgrade_write_img(pReq->upload_data, pReq->upload_len);

        sal_fwug_http_init();
        sal_fwug_image_write(pReq->upload_data, pReq->upload_len);
    }
    else
    {
        if ((pReq->upload_offset < pReq->upload_size) &&
            (HTTP_IMAGE_TRANSFERING == g_firm_update_stat))
        {
            if ((pReq->upload_offset + pReq->upload_len) >= pReq->upload_size)
            {
                /* last block */
                memcpy_len = pReq->upload_size - pReq->upload_offset;

#if 0
                DBG_HTTP_UPDATE( "write flash: [%p] ~ [%p], len = %ld (last block)\n", pReq->upload_offset, \
                    pReq->upload_offset + memcpy_len, memcpy_len);
#endif

                g_firm_update_stat = HTTP_IMAGE_FINISHED;

                // upgrade_write_img(pReq->upload_data, memcpy_len);
                // upgrade_set_img_len(pReq->upload_offset);
                // upgrade_set_status(UPGRADE_SUCCESS);
                // upgrade_set_reboot_flag(TRUE);
                sal_fwug_image_write(pReq->upload_data, pReq->upload_len);
                sal_fwug_termination_set();
            }
            else
            {
#if 0            
                DBG_HTTP_UPDATE( "write flash: [%p] ~ [%p], len = %ld\n", pReq->upload_offset, \
                    pReq->upload_offset + pReq->upload_len, pReq->upload_len);
#endif

                // upgrade_write_img(pReq->upload_data, pReq->upload_len);
                sal_fwug_image_write(pReq->upload_data, pReq->upload_len);
            }
        }
    }

#if 0
    /* show webpage at end */
    if (HTTP_IMAGE_ERROR == g_firm_update_stat)
    {
        WEB_BEGIN(pReq);
                
        WEB_STATUS(pReq, HTTP_STATUS_OK);
        WEB_HEADER(pReq, "Content-Type: text/html");
         
        WEB_DATA(pReq, htm_fw_upgrade_failed, sizeof(htm_fw_upgrade_failed));
            
        WEB_END(pReq);    
        g_firm_update_stat = HTTP_IMAGE_IDLE;
        
        return SUCCESS;
    }
    
    if (HTTP_IMAGE_FINISHED == g_firm_update_stat)
    {
        WEB_BEGIN(pReq);
                
        WEB_STATUS(pReq, HTTP_STATUS_OK);
        WEB_HEADER(pReq, "Content-Type: text/html");
       
        WEB_DATA(pReq, htm_fw_upgrade_ok, sizeof(htm_fw_upgrade_ok));
            
        WEB_END(pReq);
        g_firm_update_stat = HTTP_IMAGE_IDLE;

        return SUCCESS;
    }
#endif

    return SUCCESS;
}
