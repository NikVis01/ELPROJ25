from flask import Flask, jsonify, render_template, send_file
import json
import os
import serial
import serial.tools.list_ports
import time
import logging
import sys

# Set up logging configuration
logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")

app = Flask(__name__)

def check_arduino_connection(ser):
    """
    Check if the Arduino is connected using an open serial connection.
    """
    try:
        logging.info("Checking connection to Arduino...")
        ser.reset_input_buffer()
        logging.info("Successfully connected to Arduino.")
        return True
    except serial.SerialException as e:
        logging.error(f"Failed to connect to Arduino: {e}")
        return False

def get_stored_data(ser):
    """
    Retrieve stored data from the Arduino using an open serial connection.
    """
    try:
        ser.reset_input_buffer()  # Clear any old data in the buffer
        logging.info("Sending 'R' command to Arduino to retrieve stored data...")
        ser.write(b'R')  # Send the 'R' command to retrieve all stored data
        time.sleep(0.5)  # Small delay to allow Arduino to respond

        # Read data from the Arduino
        data = []
        logging.info("Reading data from Arduino...")
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                logging.info(f"Received line: {line}")
                if line == "END":
                    logging.info("End of data received.")
                    break
                else:
                    # Parse the data (assuming it's in CSV format: timestamp,temperature,turbidity)
                    try:
                        timestamp, temperature, turbidity = map(float, line.split(","))
                        data.append({
                            "timestamp": int(timestamp),
                            "temperature": temperature,
                            "turbidity": turbidity
                        })
                    except ValueError:
                        logging.error(f"Invalid data format: {line}")
        return data

    except serial.SerialException as e:
        logging.error(f"Serial error: {e}")
        return {"error": "Failed to connect to Arduino."}

    except Exception as e:
        logging.error(f"Unexpected error: {e}")
        return {"error": "An unexpected error occurred."}

@app.route("/")
def index():
    logging.info("Rendering index page...")
    # Render the HTML page
    return render_template("index.html")

@app.route("/data")
def data():
    logging.info("Accessing data endpoint...")
    # Return stored data as JSON
    stored_data = get_stored_data(ser)
    logging.info(f"Stored data: {stored_data}")
    return jsonify(stored_data)

@app.route("/download")
def download():
    logging.info("Accessing download endpoint...")
    # Generate a JSON file for download
    stored_data = get_stored_data(ser)

    # Get the directory of the current script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    json_file_path = os.path.join(script_dir, "data.json")

    # Write the JSON data to the file
    with open(json_file_path, "w") as f:
        json.dump(stored_data, f)

    # Use Flask's `send_file` to send the file for download
    return send_file(json_file_path, as_attachment=True)

if __name__ == "__main__":
    # Configure the serial connection (adjust the port and baudrate as needed)
    port = 'COM3'  # Replace with the correct COM port for your Arduino
    baudrate = 9600
    timeout = None  # Set to None for no timeout

    try:
        # Open the serial connection once and reuse it
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            # Check connection to Arduino before starting the Flask app
            if not check_arduino_connection(ser):
                logging.error("Unable to start the server. Ensure the Arduino is connected to the correct port.")
                sys.exit(1)

            # Start the Flask app
            app.run(debug=True)
    except serial.SerialException as e:
        logging.error(f"Failed to open serial port {port}: {e}")
        sys.exit(1)