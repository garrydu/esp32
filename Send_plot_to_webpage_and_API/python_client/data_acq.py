import requests
import xml.etree.ElementTree as ET

# URL of ESP32 XML endpoint
url="http://127.0.0.1:8180/data_acq"

response = requests.get(url)
if response.status_code == 200:
    xml_content = response.text
    print(xml_content)
    root = ET.fromstring(xml_content)
    values = [int(v.text) for v in root.findall(".//v")]
    print("Values from ESP32:", values)
else:
    print("Failed to get data from ESP32")

