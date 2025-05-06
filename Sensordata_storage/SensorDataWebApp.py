from flask import Flask, jsonify, render_template, send_file
import serial
import time
import logging
import os
import sys
import json

# Set up logging configuration to log to console
logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")

app = Flask(__name__)

# Function to read data from data.json
def read_data_from_file():
    """
    Read sensor data from the data.json file.
    """
    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        json_file_path = os.path.join(script_dir, "data.json")
        logging.info(f"Reading data from {json_file_path}...")
        
        with open(json_file_path, "r") as f:
            data = json.load(f)
            logging.info("Successfully read data from file.")
            return data
    except FileNotFoundError:
        logging.error("data.json file not found.")
        return {"error": "data.json file not found."}
    except json.JSONDecodeError as e:
        logging.error(f"Error decoding JSON file: {e}")
        return {"error": "Error decoding JSON file."}
    except Exception as e:
        logging.error(f"Unexpected error reading data.json: {e}")
        return {"error": "Unexpected error occurred."}

# ArduinoDataRetriever class remains unchanged for easy reactivation
class ArduinoDataRetriever:
    def __init__(self, port='COM3', baudrate=9600, timeout=10):
        try:
            logging.info(f"Initializing connection to Arduino on port {port}...")
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            self.ser.reset_input_buffer()  # Clears cached data in the serial buffer
            time.sleep(5)  # Wait for Arduino to reset
            logging.info("Successfully connected to Arduino.")
        except serial.SerialException as e:
            logging.error(f"Failed to connect to Arduino: {e}")
            raise

    def retrieve_data(self):
        """
        Retrieve all stored data from the Arduino using the 'D' command.
        The data is expected to be in CSV format: timestamp,temperature,turbidity.
        """
        try:
            self.ser.reset_input_buffer()
            self.ser.write(b'D')  # Command to retrieve all stored data
            time.sleep(0.5)  # Small delay to let Arduino respond

            logging.info("Retrieving stored data from Arduino...")
            data = []
            while True:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').strip()
                    logging.debug(f"Received line: {line}")
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
        except Exception as e:
            logging.error(f"Error retrieving data: {e}")
            raise

@app.route("/")
def index():
    logging.info("Rendering index page...")
    return render_template("index.html")

@app.route("/data")
def data():
    """
    Serve data from the data.json file.
    """
    logging.info("Accessing /data endpoint...")
    try:
        stored_data = read_data_from_file()
        if "error" in stored_data:
            return jsonify(stored_data), 500  # Return error with HTTP 500 status
        return jsonify(stored_data)
    except Exception as e:
        logging.error(f"Error serving data: {e}")
        return jsonify({"error": "Failed to serve data."}), 500

@app.route("/download")
def download():
    """
    Serve the data.json file for download.
    """
    logging.info("Accessing /download endpoint...")
    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        json_file_path = os.path.join(script_dir, "data.json")
        return send_file(json_file_path, as_attachment=True)
    except FileNotFoundError:
        logging.error("data.json file not found.")
        return jsonify({"error": "data.json file not found."}), 404
    except Exception as e:
        logging.error(f"Error serving data.json for download: {e}")
        return jsonify({"error": "Failed to serve data.json for download."}), 500

# function that turns turbidity reading from 740-0 to 100-0. Capped at 100 and 0
def convertTurbidity(turbidity): 
  converted = map(turbidity, 0, 740, 0, 100)
  
  if converted > 100: 
      converted = 100
  elif converted < 0: 
    converted = 0
  
  return converted

if __name__ == "__main__":
    app.run(debug=True)