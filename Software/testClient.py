import requests

# Replace with the IP address of your ESP8266 server
esp8266_ip = '10.0.0.31'
url = f'http://{esp8266_ip}/'

try:
    response = requests.get(url)
    print(f"Response Status Code: {response.status_code}")
    print("Response Text:")
    print(response.text)
except requests.exceptions.RequestException as e:
    print(f"Request failed: {e}")