//#include <string.h>
//#include <stdio.h>

//#include <salflash.h>
//#include <salsys.h>

//#include <web/web_api.h>
//#include <syslog/syslogdefs.h>

//int32 get_poe(struct http_request_s *pReq)
//{
//	 uint8 username[SAL_FLSH_USR_USERNAME_LEN + 1];
//    uint8 password[SAL_FLSH_USR_PASSWORD_LEN + 1];

//    sal_flash_sysAccount_get(username, password);

//    WEB_BEGIN(pReq);

//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: text/html");
//WEB_PRINTF(pReq, "<html lang=\"en\">\n");
//WEB_PRINTF(pReq, "<head>\n");
//WEB_PRINTF(pReq, "<meta charset=\"UTF-8\">\n");
//WEB_PRINTF(pReq, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
//WEB_PRINTF(pReq, "<title>POE NEW</title>\n");
//WEB_PRINTF(pReq, "</head>\n");
//WEB_PRINTF(pReq, "<script>\n");
//WEB_PRINTF(pReq, "let fetchFlag=true;\n");
//WEB_PRINTF(pReq, "let fetchInterval;\n");
//WEB_PRINTF(pReq, "const ip = window.location.hostname;\n");
//WEB_PRINTF(pReq, "async function getPoeData() {\n");
//WEB_PRINTF(pReq, "try {\n");
//WEB_PRINTF(pReq, "let response = await fetch(`http://${ip}:80/get_poe_status`);\n");
//WEB_PRINTF(pReq, "let res1 = await response.json();\n");
//WEB_PRINTF(pReq, "let res = { \"ac\": res1.A, \"battery\": res1.B, \"batteryPercentage\": res1.C, \"mode\": res1.D, \"voltage\": res1.E };\n");
//WEB_PRINTF(pReq, "const acLight = document.getElementById('ac-light');\n");
//WEB_PRINTF(pReq, "if (res.ac === 1) {\n");
//WEB_PRINTF(pReq, "acLight.style.backgroundColor = 'green';\n");
//WEB_PRINTF(pReq, "} else {\n");
//WEB_PRINTF(pReq, "acLight.style.backgroundColor = 'red';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "const batteryLight = document.getElementById('battery-light');\n");
//WEB_PRINTF(pReq, "if (res.battery === 1) {\n");
//WEB_PRINTF(pReq, "batteryLight.style.backgroundColor = 'green';\n");
//WEB_PRINTF(pReq, "} else {\n");
//WEB_PRINTF(pReq, "batteryLight.style.backgroundColor = 'red';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "const batteryParts = document.querySelectorAll('.battery-part');\n");
//WEB_PRINTF(pReq, "const batteryPercentage = res.batteryPercentage || 0;\n");
//WEB_PRINTF(pReq, "const segmentsToFill = Math.floor(batteryPercentage / 25);\n");
//WEB_PRINTF(pReq, "batteryParts.forEach((part, index) => {\n");
//WEB_PRINTF(pReq, "if (index < segmentsToFill) {\n");
//WEB_PRINTF(pReq, "part.style.backgroundColor = 'green';\n");
//WEB_PRINTF(pReq, "} else { part.style.backgroundColor = '#e0e0e0';}\n");
//WEB_PRINTF(pReq, "});\n");
//WEB_PRINTF(pReq, "if (batteryPercentage === 25) {\n");
//WEB_PRINTF(pReq, "batteryParts[0].style.backgroundColor = 'yellow';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "const mode = {0: \"Low\",1: \"Normal\",2: \"High\"};\n");
//WEB_PRINTF(pReq, "const inputField = document.querySelector('.ac-supply input');\n");
//WEB_PRINTF(pReq, "if (inputField && res.mode !== undefined) {\n");
//WEB_PRINTF(pReq, "inputField.value = mode[res.mode] || \"Unknown\";\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "const voltageCheckboxes = document.querySelectorAll('.checkbox-group input[type=\"checkbox\"]');\n");
//WEB_PRINTF(pReq, "voltageCheckboxes.forEach((checkbox) => {\n");
//WEB_PRINTF(pReq, "if (checkbox.value === `${res.voltage}V`) {\n");
//WEB_PRINTF(pReq, "checkbox.checked = true;\n");
//WEB_PRINTF(pReq, "checkbox.nextElementSibling.style.backgroundColor = 'green';\n");
//WEB_PRINTF(pReq, "} else {\n");
//WEB_PRINTF(pReq, "checkbox.checked = false;\n");
//WEB_PRINTF(pReq, "checkbox.nextElementSibling.style.backgroundColor = 'white';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "});\n");
//WEB_PRINTF(pReq, "} catch (error) {\n");
//WEB_PRINTF(pReq, "console.log(\"Error fetching data:\", error);\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "function handleVoltageSelection(event) {\n");
//WEB_PRINTF(pReq, "const checkboxes = document.querySelectorAll('.checkbox-group input[type=\"checkbox\"]');\n");
//WEB_PRINTF(pReq, "checkboxes.forEach(checkbox => {\n");
//WEB_PRINTF(pReq, "if (checkbox !== event.target) {\n");
//WEB_PRINTF(pReq, "checkbox.checked = false;\n");
//WEB_PRINTF(pReq, "checkbox.nextElementSibling.style.backgroundColor = 'white';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "});\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "function toggleCheckbox(checkbox) {\n");
//WEB_PRINTF(pReq, "console.log(\"toggleCheckbox\");\n");
//WEB_PRINTF(pReq, "fetchFlag=false;\n");
//WEB_PRINTF(pReq, "setTimeout(()=>{fetchFlag=true}, 6000);\n");
//WEB_PRINTF(pReq, "const box = checkbox.nextElementSibling;\n");
//WEB_PRINTF(pReq, "if (checkbox.checked) {\n");
//WEB_PRINTF(pReq, "box.style.backgroundColor = 'green'; // green when checked\n");
//WEB_PRINTF(pReq, "} else {\n");
//WEB_PRINTF(pReq, "box.style.backgroundColor = 'white';\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "async function applyVoltage() {\n");
//WEB_PRINTF(pReq, "const selectedVoltage = document.querySelector('.checkbox-group input[type=\"checkbox\"]:checked');\n");
//WEB_PRINTF(pReq, "if (selectedVoltage) {\n");
//WEB_PRINTF(pReq, "try {\n");
//WEB_PRINTF(pReq, "const buttonHtml = document.getElementById('applyButton');\n");
//WEB_PRINTF(pReq, "buttonHtml.disabled = true;\n");
//WEB_PRINTF(pReq, "buttonHtml.style.backgroundColor = 'gray';\n");
//WEB_PRINTF(pReq, "buttonHtml.innerHTML = 'Loading...';\n");
//WEB_PRINTF(pReq, "let response;\n");
//WEB_PRINTF(pReq, "if (selectedVoltage.value === '24V') {\n");
//WEB_PRINTF(pReq, "response = await fetch(`http://${ip}/set_24`);\n");
//WEB_PRINTF(pReq, "} else if (selectedVoltage.value === '30V') {\n");
//WEB_PRINTF(pReq, "response = await fetch(`http://${ip}/set_30`);\n");
//WEB_PRINTF(pReq, "} else if (selectedVoltage.value === '48V') {\n");
//WEB_PRINTF(pReq, "response = await fetch(`http://${ip}/set_48`);\n");
//WEB_PRINTF(pReq, "} else if (selectedVoltage.value === '56V') {\n");
//WEB_PRINTF(pReq, "response = await fetch(`http://${ip}/set_56`);\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "buttonHtml.disabled = false;\n");
//WEB_PRINTF(pReq, "buttonHtml.style.backgroundColor = 'blue';\n");
//WEB_PRINTF(pReq, "buttonHtml.innerHTML = 'Apply';\n");
//WEB_PRINTF(pReq, "if (response.ok) {\n");
//WEB_PRINTF(pReq, "alert(`Voltage set successfully to ${selectedVoltage.value}`);\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "} catch (error) {\n");
//WEB_PRINTF(pReq, "console.error(\"Error setting voltage:\", error);\n");
//WEB_PRINTF(pReq, "alert(`Failed to set voltage due to a network error: ${error.message}`);\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "} else {\n");
//WEB_PRINTF(pReq, "alert('Please select a voltage option.');\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "async function startTimer(){\n");
//WEB_PRINTF(pReq, "fetchInterval = setInterval(()=>{\n");
//WEB_PRINTF(pReq, "if(fetchFlag){ getPoeData();}}, 7000);\n");
//WEB_PRINTF(pReq, "}\n");
//WEB_PRINTF(pReq, "getPoeData();\n");
//WEB_PRINTF(pReq, "setTimeout(getPoeData, 1800);\n");
//WEB_PRINTF(pReq, "setTimeout(getPoeData, 1800);\n");
//WEB_PRINTF(pReq, "setTimeout(getPoeData, 1800);\n");
//WEB_PRINTF(pReq, "setTimeout(getPoeData, 1800);\n");
//WEB_PRINTF(pReq, "startTimer();\n");
//WEB_PRINTF(pReq, "</script>\n");
//WEB_PRINTF(pReq, "<body>\n");
//WEB_PRINTF(pReq, "<div style=\"margin: auto 10px; font-family: Arial, sans-serif;  \">\n");
//WEB_PRINTF(pReq, "<h1 style=\"text-align: center; margin: 20px auto; font-size: 24px; color: #333;\">POE</h1>\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"display: flex; flex-direction: row; justify-content: space-between; align-items: center; width: 500px; margin: 50px auto; padding: 30px; background-color: rgb(208, 215, 177); border-radius: 10px; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);\">\n");
//WEB_PRINTF(pReq, "<div style=\"display: flex; flex-direction: column; gap: 10px;\">\n");
//WEB_PRINTF(pReq, "<div class=\"checkbox-group\">\n");
//WEB_PRINTF(pReq, "<label style=\"margin-right: 10px; font-size: 16px;\">\n");
//WEB_PRINTF(pReq, "<input type=\"checkbox\" value=\"56V\" style=\"display: none;\"\n");
//WEB_PRINTF(pReq, "onclick=\"handleVoltageSelection(event); toggleCheckbox(this)\">\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"width: 20px; height: 20px; border-radius: 5px; background-color: white; transition: background-color 0.3s; cursor: pointer;\">\n");
//WEB_PRINTF(pReq, "</div> 56V\n");
//WEB_PRINTF(pReq, "</label>\n");
//WEB_PRINTF(pReq, "<label style=\"margin-right: 10px; font-size: 16px;\">\n");
//WEB_PRINTF(pReq, "<input type=\"checkbox\" value=\"48V\" style=\"display: none;\"\n");
//WEB_PRINTF(pReq, "onclick=\"handleVoltageSelection(event); toggleCheckbox(this)\">\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"width: 20px; height: 20px; border-radius: 5px; background-color: white; transition: background-color 0.3s; cursor: pointer;\">\n");
//WEB_PRINTF(pReq, "</div> 48V\n");
//WEB_PRINTF(pReq, "</label>\n");
//WEB_PRINTF(pReq, "<label style=\"margin-right: 10px; font-size: 16px;\">\n");
//WEB_PRINTF(pReq, "<input type=\"checkbox\" value=\"30V\" style=\"display: none;\"\n");
//WEB_PRINTF(pReq, "onclick=\"handleVoltageSelection(event); toggleCheckbox(this)\">\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"width: 20px; height: 20px; border-radius: 5px; background-color: white; transition: background-color 0.3s; cursor: pointer;\">\n");
//WEB_PRINTF(pReq, "</div> 30V\n");
//WEB_PRINTF(pReq, "</label>\n");
//WEB_PRINTF(pReq, "<label style=\"margin-right: 10px; font-size: 16px;\">\n");
//WEB_PRINTF(pReq, "<input type=\"checkbox\" value=\"24V\" style=\"display: none;\"\n");
//WEB_PRINTF(pReq, "onclick=\"handleVoltageSelection(event); toggleCheckbox(this)\">\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"width: 20px; height: 20px; border-radius: 5px; background-color: white; transition: background-color 0.3s; cursor: pointer;\">\n");
//WEB_PRINTF(pReq, "</div> 24V\n");
//WEB_PRINTF(pReq, "</label>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<button id=\"applyButton\" onclick=\"applyVoltage()\"\n");
//WEB_PRINTF(pReq, "style=\"background-color: #0056b3; color: white; padding: 5px 10px; border: none; border-radius: 5px; cursor: pointer;\">Apply</button>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div style=\"display: flex; flex-direction: column; gap: 10px;\">\n");
//WEB_PRINTF(pReq, "<div style=\"display: flex; gap: 10px; justify-content: start; align-items: center;\">\n");
//WEB_PRINTF(pReq, "<div id=\"ac-light\"\n");
//WEB_PRINTF(pReq, "style=\"width: 15px; height: 15px; border-radius: 50%; background-color: green; box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div>AC</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div style=\"display: flex; gap: 10px; justify-content: start; align-items: center;\">\n");
//WEB_PRINTF(pReq, "<div id=\" -light\"\n");
//WEB_PRINTF(pReq, "style=\"width: 15px; height: 15px; border-radius: 50%; background-color: green; box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div>Battery</div>\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"display: flex; width: 100px; height: 30px; border: 2px solid #333; border-radius: 5px; background-color: #fff; position: relative;\">\n");
//WEB_PRINTF(pReq, "<div class=\"battery-part\" style=\"width: 25%; background-color: #e0e0e0; border-right: 2px solid #333;\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div class=\"battery-part\" style=\"width: 25%; background-color: #e0e0e0; border-right: 2px solid #333;\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div class=\"battery-part\" style=\"width: 25%; background-color: #e0e0e0; border-right: 2px solid #333;\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div class=\"battery-part\" style=\"width: 25%;  background-color: #e0e0e0;\"></div>\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"width: 10px; height: 10px; background-color: #333; position: absolute; right: -12px; top: 10%; transform: translateY(-50%); border-radius: 3px;\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div\n");
//WEB_PRINTF(pReq, "style=\"position: absolute; top: 10%; left: 20%; transform: translate(-50%, -50%); font-size: 14px; color: #333;\">\n");
//WEB_PRINTF(pReq, "Battery</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "<div style=\"display: flex; justify-content: flex-start; align-items: center; gap: 10px; margin-top: 10px;\"\n");
//WEB_PRINTF(pReq, "class=\"ac-supply\">\n");
//WEB_PRINTF(pReq, "<p>AC Supply:</p>\n");
//WEB_PRINTF(pReq, "<input type=\"text\" readonly\n");
//WEB_PRINTF(pReq, "style=\"height: 20px; width: 100px; border: 1px solid white; border-radius: 5px; padding: 2px 5px; text-align: center; font-size: 14px;\">\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "</div>\n");
//WEB_PRINTF(pReq, "</body>\n");
//WEB_PRINTF(pReq, "</html>\n");
// 
//    WEB_END(pReq);

//	return SUCCESS;
//}






