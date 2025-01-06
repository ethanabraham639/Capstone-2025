import requests
import time

# Define the base URL of the server
BASE_URL = "http://192.168.4.1"

def course_state_post():
    """Function to perform a POST request to /course_state."""
    integers = [0] + list(range(45))
    string = ''.join(chr(value) for value in integers)
    print(string)
    response = requests.post(f"{BASE_URL}/course_state", data=string)
    print("POST /course_state response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

def reset_stats_post():
    """Function to perform a POST request to /reset_stats."""
    response = requests.post(f"{BASE_URL}/reset_stats")
    print("POST /reset_stats response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

def settings_post():
    """Function to perform a POST request to /settings."""
    integers = [0]
    string = ''.join(chr(value) for value in integers)
    print(string)
    response = requests.post(f"{BASE_URL}/settings", data=string)
    print("POST /settings response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

def dispense_ball_post():
    """Function to perform a POST request to /dispense_ball."""
    integers = [69]
    string = ''.join(chr(value) for value in integers)
    print(string)
    response = requests.post(f"{BASE_URL}/dispense_ball", data=string)
    print("POST /dispense_ball response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)



def error_codes_get():
    """Function to perform a GET request to /error_codes."""
    response = requests.get(f"{BASE_URL}/error_codes")
    print("GET /error_codes response:")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)

def debug_msg_get():
    """Function to perform a GET request to /debug_msg."""
    response = requests.get(f"{BASE_URL}/debug_msg")
    print("GET /debug_message response:")
    print("Status Code:", response.status_code)
    print("Debug Msg:", response.text)

def stats_get():
    """Function to perform a GET request to /stats."""
    response = requests.get(f"{BASE_URL}/stats")
    print("GET /stats response:")
    print("Status Code:", response.status_code)
    print(f"Balls hit: {ord(response.text[0])}, Balls in hole: {ord(response.text[1])}")

if __name__ == "__main__":
    error_codes_get()
    print()
    debug_msg_get()
    print()
    stats_get()
    print()

    course_state_post()
    print()
    reset_stats_post()
    print()
    settings_post()
    print()
    dispense_ball_post()