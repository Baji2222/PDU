#pragma STRING(far)

#include <web/web_api.h>
#include <web/web_err.h>
#include <web/http_debug.h>

#include "web_menu.h"

//uint8 const far htm_menu[] = "\
//<html>\n\
//<head>\n\
//<title>Menu</title>\n\
//<STYLE type=\"text/css\">\n\
//*{margin:0;padding:0;box-sizing:border-box}\n\
//body{font-size:13px;font-family:sans-serif;background:#f0f0f0;height:100vh;overflow:hidden}\n\
//.header{display:flex;align-items:center;background:#0f1a2b;padding:0 25px;height:70px;width:100%;position:fixed;top:0;left:0;z-index:100;border-bottom:2px solid #00d4ff}\n\
//.logo-container{flex:0 0 auto;margin-right:20px}\n\
//.logo-container img{height:45px;vertical-align:middle}\n\
//.title-container{flex:1;text-align:center;color:#fff;font-size:22px;font-weight:700;letter-spacing:4px;text-transform:uppercase}\n\
//.title-container span{color:#00d4ff}\n\
//.header-status{display:flex;align-items:center;gap:15px;font-size:11px;color:#8899b4}\n\
//.status-dot{display:inline-block;width:8px;height:8px;border-radius:50%;background:#00ff88}\n\
//.sidebar{position:fixed;top:70px;left:0;width:220px;height:calc(100vh - 70px);background:#0f1a2b;overflow-y:auto;z-index:50;border-right:1px solid #1a2f44}\n\
//.sidebar ul{list-style:none;margin:0;padding:10px 0}\n\
//.sidebar ul li{border-bottom:1px solid rgba(26,47,68,0.5);position:relative}\n\
//.sidebar ul li a{display:block;color:#8899b4;padding:14px 20px 14px 25px;text-decoration:none;font-size:13px;font-weight:500}\n\
//.sidebar ul li a:hover{color:#fff;background:rgba(0,212,255,0.05);padding-left:30px}\n\
//.sidebar ul li.active a{color:#00d4ff;background:rgba(0,212,255,0.08)}\n\
//.sidebar ul ul{display:none;background:rgba(0,0,0,0.2);padding:0}\n\
//.sidebar ul ul li{border-bottom:1px solid rgba(26,47,68,0.3)}\n\
//.sidebar ul ul li a{padding:12px 20px 12px 40px;font-size:12px;color:#6a7b94}\n\
//.sidebar ul ul li a:hover{color:#fff;padding-left:45px}\n\
//.sidebar ul li:hover>ul{display:block}\n\
//.sidebar-footer{position:absolute;bottom:0;left:0;width:100%;padding:15px 20px;background:#0a0e17;border-top:1px solid #1a2f44}\n\
//.sidebar-footer .model{color:#00d4ff;font-weight:600;font-size:11px;margin-bottom:3px}\n\
//.sidebar-footer .fw{color:#4a5a72;font-size:10px}\n\
//.content-container{position:fixed;top:70px;left:220px;width:calc(100% - 220px);height:calc(100vh - 70px);background:#fff;overflow:hidden}\n\
//iframe{width:100%;height:100%;border:none;display:block;background:#fff}\n\
//.sidebar::-webkit-scrollbar{width:4px}\n\
//.sidebar::-webkit-scrollbar-track{background:#0f1a2b}\n\
//.sidebar::-webkit-scrollbar-thumb{background:#00d4ff;border-radius:2px}\n\
//</STYLE>\n\
//<script>\n\
//function h(){var a=document.getElementById('f').contentWindow.location.pathname,b=document.querySelectorAll('.sidebar>ul>li>a');b.forEach(function(d){d.parentElement.classList.remove('active');var e=d.getAttribute('href');if(e===a||e===a.replace('./','')){d.parentElement.classList.add('active')}})}\n\
//window.onload=function(){setTimeout(h,300);document.getElementById('f').addEventListener('load',h)}\n\
//</script>\n\
//</head>\n\
//<body>\n\
//<div class=\"header\">\n\
//<div class=\"logo-container\"><a href='http://www.opplin.com' target='_blank'><img src='/logo.png' border=0 alt='OPL'></a></div>\n\
//<div class=\"title-container\"><span>SMART</span> PDU</div>\n\
//<div class=\"header-status\"><span class=\"status-dot\"></span><span>ONLINE</span><span style=\"color:#3a4a62;\">|</span><span>v1.03</span></div>\n\
//</div>\n\
//<div class=\"sidebar\">\n\
//<ul>\n\
//<li><a href=\"poe.cgi\" target=\"f\">PDU Dashboard</a></li>\n\
//<li><a href=\"info.cgi\" target=\"f\">Information</a></li>\n\
//<li><a href=\"ip.cgi\" target=\"f\">IP Setting</a></li>\n\
//<li><a href=\"vlan.cgi?page=static\" target=\"f\">Static VLAN</a></li>\n\
//<li><a href=\"vlan.cgi?page=port_based\" target=\"f\">VLAN Setting</a></li>\n\
//<li><a href=\"vlan.cgi?page=vlan_ip\" target=\"f\">VLAN IP</a></li>\n\
//<li><a href=\"user.cgi\" target=\"f\">User Account</a></li>\n\
//<li><a href=\"snmpsecurity.cgi?page=snmpsec\" target=\"f\">SNMP Settings</a></li>\n\
//<li><a href=\"#\">Tools</a>\n\
//<ul>\n\
//<li><a href=\"fwug.cgi\" target=\"f\">Firmware Upgrade</a></li>\n\
//<li><a href=\"config_back.cgi\" target=\"f\">Configuration Backup</a></li>\n\
//<li><a href=\"reset.cgi\" target=\"f\">Reset</a></li>\n\
//<li><a href=\"save.cgi\" target=\"f\">Save</a></li>\n\
//<li><a href=\"reboot.cgi\" target=\"f\">Reboot</a></li>\n\
//</ul>\n\
//</li>\n\
//</ul>\n\
//<div class=\"sidebar-footer\">\n\
//<div class=\"model\">MODEL:OPL-PDU8-C20</div>\n\
//<div class=\"fw\">FW:1.03.12212017</div>\n\
//</div>\n\
//</div>\n\
//<div class=\"content-container\">\n\
//<iframe id=\"f\" name=\"f\" src=\"poe.cgi\"></iframe>\n\
//</div>\n\
//</body>\n\
//</html>\n\
//";


//uint8 const far htm_menu[] = "\
//<html>\n\
//<head>\n\
//<title>Menu</title>\n\
//<STYLE type=\"text/css\">\n\
//*{margin:0;padding:0;box-sizing:border-box}\n\
//body{font-size:13px;font-family:sans-serif;background:#f0f0f0;height:100vh;overflow:hidden}\n\
//.header{display:flex;align-items:center;background:#0f1a2b;padding:0 25px;height:70px;width:100%;position:fixed;top:0;left:0;z-index:100;border-bottom:2px solid #00d4ff}\n\
//.logo-container{flex:0 0 auto;margin-right:20px}\n\
//.logo-container img{height:45px;vertical-align:middle}\n\
//.title-container{flex:1;text-align:center;color:#fff;font-size:22px;font-weight:700;letter-spacing:4px;text-transform:uppercase}\n\
//.title-container span{color:#00d4ff}\n\
//.header-status{display:flex;align-items:center;gap:15px;font-size:11px;color:#8899b4}\n\
//.status-dot{display:inline-block;width:8px;height:8px;border-radius:50%;background:#00ff88}\n\
//.sidebar{position:fixed;top:70px;left:0;width:220px;height:calc(100vh - 70px);background:#0f1a2b;overflow-y:auto;z-index:50;border-right:1px solid #1a2f44}\n\
//.sidebar ul{list-style:none;margin:0;padding:10px 0}\n\
//.sidebar ul li{border-bottom:1px solid rgba(26,47,68,0.5);position:relative}\n\
//.sidebar ul li a{display:block;color:#8899b4;padding:14px 20px 14px 25px;text-decoration:none;font-size:13px;font-weight:500;cursor:pointer}\n\
//.sidebar ul li a:hover{color:#fff;background:rgba(0,212,255,0.05);padding-left:30px}\n\
//.sidebar ul li.active a{color:#00d4ff;background:rgba(0,212,255,0.08)}\n\
//.sidebar ul ul{display:none;background:rgba(0,0,0,0.2);padding:0}\n\
//.sidebar ul ul li{border-bottom:1px solid rgba(26,47,68,0.3)}\n\
//.sidebar ul ul li a{padding:12px 20px 12px 40px;font-size:12px;color:#6a7b94}\n\
//.sidebar ul ul li a:hover{color:#fff;padding-left:45px}\n\
//.sidebar ul li:hover>ul{display:block}\n\
//.sidebar-footer{position:absolute;bottom:0;left:0;width:100%;padding:15px 20px;background:#0a0e17;border-top:1px solid #1a2f44}\n\
//.sidebar-footer .model{color:#00d4ff;font-weight:600;font-size:11px;margin-bottom:3px}\n\
//.sidebar-footer .fw{color:#4a5a72;font-size:10px}\n\
//.content-container{position:fixed;top:70px;left:220px;width:calc(100% - 220px);height:calc(100vh - 70px);background:#fff;overflow:hidden}\n\
//iframe{width:100%;height:100%;border:none;display:block;background:#fff}\n\
//.sidebar::-webkit-scrollbar{width:4px}\n\
//.sidebar::-webkit-scrollbar-track{background:#0f1a2b}\n\
//.sidebar::-webkit-scrollbar-thumb{background:#00d4ff;border-radius:2px}\n\
//</STYLE>\n\
//<script>\n\
//function setActive(li){\n\
//    var items=document.querySelectorAll('.sidebar>ul>li');\n\
//    items.forEach(function(i){i.classList.remove('active')});\n\
//    if(li) li.classList.add('active');\n\
//}\n\
//function handleClick(e){\n\
//    var a=e.target;\n\
//    while(a.tagName!='A') a=a.parentElement;\n\
//    var li=a.parentElement;\n\
//    while(li.tagName!='LI') li=li.parentElement;\n\
//    var parentLi=li.closest('.sidebar>ul>li');\n\
//    if(parentLi) setActive(parentLi);\n\
//    else setActive(li);\n\
//}\n\
//function highlightCurrent(){\n\
//    var f=document.getElementById('f');\n\
//    if(!f||!f.contentWindow)return;\n\
//    try{\n\
//        var url=f.contentWindow.location.pathname+f.contentWindow.location.search;\n\
//        var items=document.querySelectorAll('.sidebar>ul>li');\n\
//        items.forEach(function(li){\n\
//            var a=li.querySelector('a');\n\
//            if(!a)return;\n\
//            var href=a.getAttribute('href');\n\
//            if(href && href!='#' && url.indexOf(href)!==-1){\n\
//                setActive(li);\n\
//            }\n\
//        });\n\
//        var tools=document.querySelector('.sidebar>ul>li:last-child');\n\
//        if(tools){\n\
//            var subItems=tools.querySelectorAll('ul li a');\n\
//            subItems.forEach(function(a){\n\
//                var href=a.getAttribute('href');\n\
//                if(href && url.indexOf(href)!==-1){\n\
//                    setActive(tools);\n\
//                }\n\
//            });\n\
//        }\n\
//    }catch(e){}\n\
//}\n\
//window.onload=function(){\n\
//    var links=document.querySelectorAll('.sidebar>ul>li>a, .sidebar>ul>li>ul li a');\n\
//    links.forEach(function(a){\n\
//        a.addEventListener('click',handleClick);\n\
//    });\n\
//    setTimeout(highlightCurrent,200);\n\
//    var f=document.getElementById('f');\n\
//    if(f){\n\
//        f.onload=function(){setTimeout(highlightCurrent,100);};\n\
//    }\n\
//};\n\
//</script>\n\
//</head>\n\
//<body>\n\
//<div class=\"header\">\n\
//<div class=\"logo-container\"><a href='http://www.opplin.com' target='_blank'><img src='/logo.png' border=0 alt='OPL'></a></div>\n\
//<div class=\"title-container\"><span>SMART</span> PDU</div>\n\
//<div class=\"header-status\"><span class=\"status-dot\"></span><span>ONLINE</span><span style=\"color:#3a4a62;\">|</span><span>v1.03</span></div>\n\
//</div>\n\
//<div class=\"sidebar\">\n\
//<ul>\n\
//<li><a href=\"poe.cgi\" target=\"f\">PDU Dashboard</a></li>\n\
//<li><a href=\"info.cgi\" target=\"f\">Information</a></li>\n\
//<li><a href=\"ip.cgi\" target=\"f\">IP Setting</a></li>\n\
//<li><a href=\"vlan.cgi?page=static\" target=\"f\">Static VLAN</a></li>\n\
//<li><a href=\"vlan.cgi?page=port_based\" target=\"f\">VLAN Setting</a></li>\n\
//<li><a href=\"vlan.cgi?page=vlan_ip\" target=\"f\">VLAN IP</a></li>\n\
//<li><a href=\"user.cgi\" target=\"f\">User Account</a></li>\n\
//<li><a href=\"snmpsecurity.cgi?page=snmpsec\" target=\"f\">SNMP Settings</a></li>\n\
//<li><a href=\"#\">Tools</a>\n\
//<ul>\n\
//<li><a href=\"fwug.cgi\" target=\"f\">Firmware Upgrade</a></li>\n\
//<li><a href=\"config_back.cgi\" target=\"f\">Configuration Backup</a></li>\n\
//<li><a href=\"reset.cgi\" target=\"f\">Reset</a></li>\n\
//<li><a href=\"save.cgi\" target=\"f\">Save</a></li>\n\
//<li><a href=\"reboot.cgi\" target=\"f\">Reboot</a></li>\n\
//</ul>\n\
//</li>\n\
//</ul>\n\
//<div class=\"sidebar-footer\">\n\
//<div class=\"model\">MODEL:OPL-PDU8-C20</div>\n\
//<div class=\"fw\">FW:1.03.12212017</div>\n\
//</div>\n\
//</div>\n\
//<div class=\"content-container\">\n\
//<iframe id=\"f\" name=\"f\" src=\"poe.cgi\"></iframe>\n\
//</div>\n\
//</body>\n\
//</html>\n\
//";





uint8 const far htm_menu[] = "\
<html><head><title>Menu</title><STYLE type=\"text/css\">\
*{margin:0;padding:0;box-sizing:border-box}\
body{font-size:13px;font-family:sans-serif;background:#f0f0f0;height:100vh;overflow:hidden}\
.header{display:flex;align-items:center;background:#0f1a2b;padding:0 25px;height:70px;width:100%;position:fixed;top:0;left:0;z-index:100;border-bottom:2px solid #00d4ff}\
.logo-container{flex:0 0 auto;margin-right:20px}\
.logo-container img{height:45px;vertical-align:middle}\
.title-container{flex:1;text-align:center;color:#fff;font-size:22px;font-weight:700;letter-spacing:4px;text-transform:uppercase}\
.title-container span{color:#00d4ff}\
.header-status{display:flex;align-items:center;gap:15px;font-size:11px;color:#8899b4}\
.status-dot{display:inline-block;width:8px;height:8px;border-radius:50%;background:#00ff88}\
.sidebar{position:fixed;top:70px;left:0;width:220px;height:calc(100vh - 70px);background:#0f1a2b;overflow-y:auto;z-index:50;border-right:1px solid #1a2f44}\
.sidebar ul{list-style:none;margin:0;padding:10px 0}\
.sidebar ul li{border-bottom:1px solid rgba(26,47,68,0.5)}\
.sidebar ul li a{display:block;color:#8899b4;padding:14px 20px 14px 25px;text-decoration:none;font-size:13px;font-weight:500;cursor:pointer}\
.sidebar ul li a:hover{color:#fff;background:rgba(0,212,255,0.05);padding-left:30px}\
.sidebar ul li.active>a{color:#00d4ff;background:rgba(0,212,255,0.08)}\
.sidebar ul li.active>ul li a{color:#8899b4}\
.sidebar ul li.active>ul li.active>a{color:#00d4ff;background:rgba(0,212,255,0.08)}\
.sidebar ul ul{display:none;background:rgba(0,0,0,0.2);padding:0}\
.sidebar ul ul li{border-bottom:1px solid rgba(26,47,68,0.3)}\
.sidebar ul ul li a{padding:12px 20px 12px 40px;font-size:12px;color:#6a7b94}\
.sidebar ul ul li a:hover{color:#fff;padding-left:45px}\
.sidebar ul li:hover>ul{display:block}\
.sidebar-footer{position:absolute;bottom:0;left:0;width:100%;padding:15px 20px;background:#0a0e17;border-top:1px solid #1a2f44}\
.sidebar-footer .model{color:#00d4ff;font-weight:600;font-size:11px;margin-bottom:3px}\
.sidebar-footer .fw{color:#4a5a72;font-size:10px}\
.content-container{position:fixed;top:70px;left:220px;width:calc(100% - 220px);height:calc(100vh - 70px);background:#fff;overflow:hidden}\
iframe{width:100%;height:100%;border:none;display:block;background:#fff}\
.sidebar::-webkit-scrollbar{width:4px}\
.sidebar::-webkit-scrollbar-track{background:#0f1a2b}\
.sidebar::-webkit-scrollbar-thumb{background:#00d4ff;border-radius:2px}\
</STYLE><script>\
function sa(l){var i=document.querySelectorAll('.sidebar>ul>li');i.forEach(function(x){x.classList.remove('active')});var s=document.querySelectorAll('.sidebar>ul>li>ul li');s.forEach(function(x){x.classList.remove('active')});if(l)l.classList.add('active')}\
function hc(e){var a=e.target;while(a.tagName!='A')a=a.parentElement;var l=a.parentElement;while(l.tagName!='LI')l=l.parentElement;var p=l.closest('.sidebar>ul>li');if(p){sa(p);if(l!==p)l.classList.add('active')}else sa(l)}\
function hc2(){var f=document.getElementById('f');if(!f||!f.contentWindow)return;try{var u=f.contentWindow.location.pathname+f.contentWindow.location.search;var i=document.querySelectorAll('.sidebar>ul>li');i.forEach(function(l){var a=l.querySelector('a');if(!a)return;var h=a.getAttribute('href');if(h&&h!='#'&&u.indexOf(h)!==-1)sa(l)});var s=document.querySelectorAll('.sidebar>ul>li>ul li');s.forEach(function(l){var a=l.querySelector('a');if(!a)return;var h=a.getAttribute('href');if(h&&u.indexOf(h)!==-1){l.classList.add('active');var p=l.closest('.sidebar>ul>li');if(p)sa(p)}})}catch(e){}}\
window.onload=function(){var l=document.querySelectorAll('.sidebar>ul>li>a, .sidebar>ul>li>ul li a');l.forEach(function(a){a.addEventListener('click',hc)});setTimeout(hc2,200);var f=document.getElementById('f');if(f){f.onload=function(){setTimeout(hc2,100)}}};\
</script></head><body>\
<div class=\"header\"><div class=\"logo-container\"><a href='http://www.opplin.com' target='_blank'><img src='/logo.png' border=0 alt='OPL'></a></div><div class=\"title-container\"><span>SMART</span> PDU</div><div class=\"header-status\"><span class=\"status-dot\"></span><span>ONLINE</span><span style=\"color:#3a4a62;\">|</span><span>v1.03</span></div></div>\
<div class=\"sidebar\"><ul>\
<li><a href=\"poe.cgi\" target=\"f\">PDU Dashboard</a></li>\
<li><a href=\"info.cgi\" target=\"f\">Information</a></li>\
<li><a href=\"ip.cgi\" target=\"f\">IP Setting</a></li>\
<li><a href=\"vlan.cgi?page=get_rtc_config\" target=\"f\">RTC and Alarms</a></li>\
<li><a href=\"vlan.cgi?page=set_rtc_alarm\"  target=\"f\">RTC Set Alarm</a></li>\
<li><a href=\"vlan.cgi?page=set_rtc_data_alarm\" target=\"f\">RTC Set Time and Date</a></li>\
<li><a href=\"user.cgi\" target=\"f\">User Account</a></li>\
<li><a href=\"snmpsecurity.cgi?page=snmpsec\" target=\"f\">SNMP Settings</a></li>\
<li><a href=\"#\">Tools</a><ul>\
<li><a href=\"fwug.cgi\" target=\"f\">Firmware Upgrade</a></li>\
<li><a href=\"config_back.cgi\" target=\"f\">Configuration Backup</a></li>\
<li><a href=\"reset.cgi\" target=\"f\">Reset</a></li>\
<li><a href=\"save.cgi\" target=\"f\">Save</a></li>\
<li><a href=\"reboot.cgi\" target=\"f\">Reboot</a></li>\
</ul></li></ul>\
<div class=\"sidebar-footer\"><div class=\"model\">MODEL:OPL-PDU8-C20</div><div class=\"fw\">FW:1.03.12212017</div></div></div>\
<div class=\"content-container\"><iframe id=\"f\" name=\"f\" src=\"poe.cgi\"></iframe></div>\
</body></html>\
";

/*
 * Function: get_menu
 * ------------------
 * Serves the main navigation menu page to the client.
 * Returns complete HTML with CSS styling and dropdown menus.
 * Sets Content-Type header for HTML and sends menu data.
 * Logs debug message when accessed.
 */
int8 get_menu(struct http_request_s *pReq)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_menu()-------\n"));

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_menu, sizeof(htm_menu) - 1);

    WEB_END(pReq);

    return SUCCESS;
}