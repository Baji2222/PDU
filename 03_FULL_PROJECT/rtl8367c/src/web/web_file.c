#pragma STRING (far)

#include <stdio.h>

#include "web_file.h"
#include <web/web_api.h>
#include <web/http_debug.h>

/*
 * Array: file_style_css
 * ---------------------
 * CSS stylesheet defining table, cell, and form element styles.
 * Sets colors, fonts, borders, and layout for web interface.
 * Used for consistent styling across all web pages.
 */
uint8 const far file_style_css[] = "\
.MidSize {width:100px;}\n\
\n\
table {\n\
border-collapse:collapse;\n\
border-style:solid;\n\
border-width:1px;\n\
font-size:12px;\n\
font-weight:normal;\n\
line-height:1.6em;\n\
font-family: Geneva, Arial, Helvetica, sans-serif;\n\
letter-spacing:.45pt\n\
}\n\
\n\
td {\n\
text-align:center;\n\
}\n\
\n\
th {\n\
background: rgb(32,32,255);\n\
color: rgb(255,255,255);\n\
}\n\
\n\
select, input, legend {\n\
font-family: Geneva, Arial, Helvetica, sans-serif;\n\
letter-spacing:.45pt\n\
}\n\
\n\
select {width:100px;}\n\
";

/*
 * Array: file_xp_progress_js
 * --------------------------
 * JavaScript for animated progress bar (XP-style).
 * Creates scrolling block animation with opacity effects.
 * Provides start, pause, hide/show controls for progress indication.
 */
uint8 const far file_xp_progress_js[] = "\
var w3c=(document.getElementById)?true:false;\n\
var ie=(document.all)?true:false;\n\
var N=-1;\n\
\n\
function createBar(w,h,bgc,brdW,brdC,blkC,speed,blocks,count,action){\n\
if(ie||w3c){\n\
var t='<div id=\"_xpbar'+(++N)+'\" style=\"visibility:visible; position:relative; overflow:hidden; width:'+w+'px; height:'+h+'px; background-color:'+bgc+'; border-color:'+brdC+'; border-width:'+brdW+'px; border-style:solid; font-size:1px;\">';\n\
t+='<span id=\"blocks'+N+'\" style=\"left:-'+(h*2+1)+'px; position:absolute; font-size:1px\">';\n\
t+='<span id=\"blocks'+N+'\" style=\"left:-'+(h*2+1)+'px; position:absolute; font-size:1px\">';\n\
for(i=0;i<blocks;i++){\n\
t+='<span style=\"background-color:'+blkC+'; left:-'+((h*i)+i)+'px; font-size:1px; position:absolute; width:'+h+'px; height:'+h+'px; '\n\
t+=(ie)?'filter:alpha(opacity='+(100-i*(100/blocks))+')':'-Moz-opacity:'+((100-i*(100/blocks))/100);\n\
t+='\"></span>';\n\
}\n\
t+='</span></div>';\n\
document.write(t);\n\
var bA=(ie)?document.all['blocks'+N]:document.getElementById('blocks'+N);\n\
bA.bar=(ie)?document.all['_xpbar'+N]:document.getElementById('_xpbar'+N);\n\
bA.blocks=blocks;\n\
bA.N=N;\n\
bA.w=w;\n\
bA.h=h;\n\
bA.speed=speed;\n\
bA.ctr=0;\n\
bA.count=count;\n\
bA.action=action;\n\
bA.togglePause=togglePause;\n\
bA.showBar=function(){\n\
this.bar.style.visibility=\"visible\";\n\
}\n\
bA.hideBar=function(){\n\
this.bar.style.visibility=\"hidden\";\n\
}\n\
bA.tid=setInterval('startBar('+N+')',speed);\n\
return bA;\n\
}}\n\
\n\
function startBar(bn){\n\
var t=(ie)?document.all['blocks'+bn]:document.getElementById('blocks'+bn);\n\
if(parseInt(t.style.left)+t.h+1-(t.blocks*t.h+t.blocks)>t.w){\n\
t.style.left=-(t.h*2+1)+'px';\n\
t.ctr++;\n\
if(t.ctr>=t.count){\n\
eval(t.action);\n\
t.ctr=0;\n\
}}else t.style.left=(parseInt(t.style.left)+t.h+1)+'px';\n\
}\n\
\n\
function togglePause(){\n\
if(this.tid==0){\n\
this.tid=setInterval('startBar('+this.N+')',this.speed);\n\
}else{\n\
clearInterval(this.tid);\n\
this.tid=0;\n\
}}\n\
\n\
function togglePause(){\n\
if(this.tid==0){\n\
this.tid=setInterval('startBar('+this.N+')',this.speed);\n\
}else{\n\
clearInterval(this.tid);\n\
this.tid=0;\n\
}}\n\
";

/*
 * Array: file_tree_js
 * -------------------
 * JavaScript for dynamic tree menu with expand/collapse.
 * Creates hierarchical navigation with folder icons.
 * Supports adding items, toggling visibility, and unique ID generation.
 */
uint8 const far file_tree_js[] = "\n\
var iconMap = new Array();\n\
var iconList = new Array( iconMap );\n\
function Toggle(item)\n\
{\n\
    var idx = -1;\n\
    for( i = 0; i < iconList.length; i++ )\n\
    {\n\
        if( iconList[i][0] == item )\n\
        {\n\
            idx = i;\n\
            break;\n\
        }\n\
    }\n\
    if( idx < 0 )\n\
        alert( \"Could not find key in Icon List.\" );\n\
    var div=document.getElementById(\"D\"+item);\n\
    var visible=(div.style.display!=\"none\");\n\
    var key=document.getElementById(\"P\"+item);\n\
    var removeIcon = div.hasChildNodes() == false;\n\
    if( key != null )\n\
    {\n\
        if( !removeIcon )\n\
        {\n\
            if (visible)\n\
            {\n\
                div.style.display=\"none\";\n\
                key.innerHTML=\"<img src='/plus.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
            }\n\
            else\n\
            {\n\
                div.style.display=\"block\";\n\
                key.innerHTML=\"<img src='/minus.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
            }\n\
        }\n\
        else\n\
            key.innerHTML=\"<img src='/transparent.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
    }\n\
    key=document.getElementById(\"I\"+item);\n\
    if( key != null )\n\
    {\n\
        if (visible)\n\
        {\n\
            div.style.display=\"none\";\n\
            key.innerHTML=\"<img src='\"+iconList[idx][1]+\"' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
        }\n\
        else\n\
        {\n\
            div.style.display=\"block\";\n\
            key.innerHTML=\"<img src='\"+iconList[idx][2]+\"' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
        }\n\
    }\n\
}\n\
function Expand() {\n\
   divs=document.getElementsByTagName(\"DIV\");\n\
   for (i=0;i<divs.length;i++) {\n\
     divs[i].style.display=\"block\";\n\
     key=document.getElementById(\"x\" + divs[i].id);\n\
     key.innerHTML=\"<img src='/textfolder.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
   }\n\
}\n\
function Collapse() {\n\
   divs=document.getElementsByTagName(\"DIV\");\n\
   for (i=0;i<divs.length;i++) {\n\
     divs[i].style.display=\"none\";\n\
     key=document.getElementById(\"x\" + divs[i].id);\n\
     key.innerHTML=\"<img src='/folder.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";\n\
   }\n\
}\n\
function AddImage( parent, imgFileName )\n\
{\n\
    img=document.createElement(\"IMG\");\n\
    img.setAttribute( \"src\", imgFileName );\n\
    img.setAttribute( \"width\", 16 );\n\
    img.setAttribute( \"height\", 16 );\n\
    img.setAttribute( \"hspace\", 0 );\n\
    img.setAttribute( \"vspace\", 0 );\n\
    img.setAttribute( \"border\", 0 );\n\
    parent.appendChild(img);\n\
}\n\
function CreateUniqueTagName( seed )\n\
{\n\
    var tagName = seed;\n\
    var attempt = 0;\n\
    if( tagName == \"\" || tagName == null )\n\
        tagName = \"x\";\n\
    while( document.getElementById(tagName) != null )\n\
    {\n\
        tagName = \"x\" + tagName;\n\
        if( attempt++ > 50 )\n\
        {\n\
            alert( \"Cannot create unique tag name. Giving up. \\nTag = \" + tagName );\n\
            break;\n\
        }\n\
    }\n\
    return tagName;\n\
}\n\
function CreateTreeItem( parent, img1FileName, img2FileName, nodeName, url, target )\n\
{\n\
    var uniqueId = CreateUniqueTagName( nodeName );\n\
    for( i=0; i < iconList.length; i++ )\n\
        if( iconList[i][0] == uniqueId )\n\
        {\n\
            alert( \"Non unique ID in Element Map. '\" + uniqueId + \"'\" );\n\
        }\n\
    iconList[iconList.length] = new Array( uniqueId, img1FileName, img2FileName );\n\
    table = document.createElement(\"TABLE\");\n\
    if( parent != null )\n\
        parent.appendChild( table );\n\
    table.setAttribute( \"border\", 0 );\n\
    table.setAttribute( \"cellpadding\", 1 );\n\
    table.setAttribute( \"cellspacing\", 1 );\n\
    tablebody = document.createElement(\"TBODY\");\n\
    table.appendChild(tablebody);\n\
    row=document.createElement(\"TR\");\n\
    tablebody.appendChild( row );\n\
    cell=document.createElement(\"TD\");\n\
    cell.setAttribute( \"width\", 16 );\n\
    row.appendChild(cell);\n\
    a=document.createElement(\"A\");\n\
    cell.appendChild( a );\n\
    a.setAttribute( \"id\", \"P\"+uniqueId );\n\
    a.setAttribute( \"href\", \"javascript:Toggle(\\\"\"+uniqueId+\"\\\");\" );\n\
    if (url != null)\n\
        AddImage( a, \"/minus.gif\" );\n\
    else\n\
        AddImage( a, \"/plus.gif\" );\n\
    cell=document.createElement(\"TD\");\n\
    cell.setAttribute( \"width\", 16 );\n\
    row.appendChild(cell);\n\
    a=document.createElement(\"A\");\n\
    a.setAttribute( \"id\", \"I\"+uniqueId );\n\
    a.setAttribute( \"href\", \"javascript:Toggle(\\\"\"+uniqueId+\"\\\");\" );\n\
    cell.appendChild(a);\n\
    AddImage( a, img1FileName );\n\
    cell=document.createElement(\"TD\");\n\
    cell.noWrap = true;\n\
    a=document.createElement(\"A\");\n\
    a.setAttribute( \"id\", uniqueId );\n\
    cell.appendChild( a );\n\
    if( url != null )\n\
    {\n\
        a.setAttribute( \"href\", url );\n\
        if( target != null )\n\
            a.setAttribute( \"target\", target );\n\
        else\n\
            a.setAttribute( \"target\", \"_blank\" );\n\
        text=document.createTextNode( nodeName );\n\
        a.appendChild(text);\n\
    }\n\
    else\n\
    {\n\
        text=document.createTextNode( nodeName );\n\
        cell.appendChild(text);\n\
    }\n\
    row.appendChild(cell);\n\
    return CreateDiv( parent, uniqueId );\n\
}\n\
function CreateDiv( parent, id )\n\
{\n\
    div=document.createElement(\"DIV\");\n\
    if( parent != null )\n\
        parent.appendChild( div );\n\
    div.setAttribute( \"id\", \"D\"+id );\n\
    div.style.display  = \"none\";\n\
    div.style.marginLeft = \"1.5em\";\n\
    return div;\n\
}\n\
var rootCell = null;\n\
function Initialise()\n\
{\n\
    body = document.getElementsByTagName(\"body\").item(0);\n\
    body.setAttribute( \"leftmargin\", 1.5 );\n\
    body.setAttribute( \"topmargin\", 0 );\n\
    body.setAttribute( \"marginwidth\", 0 );\n\
    body.setAttribute( \"marginheight\", 0 );\n\
    table = document.createElement(\"TABLE\");\n\
    body.appendChild( table );\n\
    table.setAttribute( \"border\", 0 );\n\
    table.setAttribute( \"cellpadding\", 1 );\n\
    table.setAttribute( \"cellspacing\", 1 );\n\
    tablebody = document.createElement(\"TBODY\");\n\
    table.appendChild(tablebody);\n\
    row=document.createElement(\"TR\");\n\
    tablebody.appendChild(row);\n\
    cell=document.createElement(\"TD\");\n\
    row.appendChild(cell);\n\
    rootCell = cell;\n\
}\n\
";


/*
 * Function Declaration
 */

/*
 * Function: get_file
 * ------------------
 * Serves static web files (CSS, JavaScript) to HTTP clients.
 * Determines file type from file_idx and sends appropriate content.
 * Sets Content-Type and Content-Length headers for each file.
 */
int8 get_file(struct http_request_s *pReq, int32 file_idx)
{
    uint8 buf[32];

    if ((file_idx <= WEB_FILE_NONE) || (file_idx >= WEB_FILE_MAX))
        return FAILED;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);

    switch (file_idx)
    {
    case WEB_FILE_STYLE_CSS:
        WEB_HEADER(pReq, "Content-Type: text/html");
        sprintf(buf, "Content-Length: %u", (uint16)sizeof(file_style_css));
        WEB_HEADER(pReq, buf);
        WEB_DATA(pReq, file_style_css, sizeof(file_style_css) - 1);
        break;

    case WEB_FILE_XP_PROGRESS_JS:
        WEB_HEADER(pReq, "Content-Type: text/html");
        sprintf(buf, "Content-Length: %u", (uint16)sizeof(file_xp_progress_js));
        WEB_HEADER(pReq, buf);
        WEB_DATA(pReq, file_xp_progress_js, sizeof(file_xp_progress_js) - 1);
        break;

    case WEB_FILE_TREE_JS:
        WEB_HEADER(pReq, "Content-Type: text/html");
        sprintf(buf, "Content-Length: %u", (uint16)sizeof(file_tree_js));
        WEB_HEADER(pReq, buf);
        WEB_DATA(pReq, file_tree_js, sizeof(file_tree_js) - 1);
        break;

    default:
        printf("%s():%d Error: Web file (%d) is not exist!\n", __FILE__, (uint16)__LINE__);
        break;
    }

    WEB_END(pReq);

    return SUCCESS;
}