import requests

# Define the base URL of the server
BASE_URL = "http://10.0.0.190"

def echo_post():
    """Function to perform a POST request to /echo."""
    data = "Hello World this is a testing string"
    response = requests.post(f"{BASE_URL}/echo", data=data)
    print("POST /echo response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

def debug_msg_get():
    """Function to perform a GET request to /debug_msg."""
    response = requests.get(f"{BASE_URL}/debug_msg")
    print("POST /echo response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

if __name__ == "__main__":
    echo_post()  # Call the POST handler
    debug_msg_get()