from flask import Flask, jsonify, render_template, send_file
import json
import os

app = Flask(__name__)

# Mock function to simulate retrieving data from Arduino
def get_stored_data():
    # Replace this with your actual `retrieve_data` logic
    return [
        {"timestamp": 1620000000, "temperature": 22.75, "turbidity": 832},
        {"timestamp": 1620003600, "temperature": 22.80, "turbidity": 789},
        {"timestamp": 1620007200, "temperature": 22.85, "turbidity": 768},
    ]

@app.route("/")
def index():
    # Render the HTML page
    return render_template("index.html")

@app.route("/data")
def data():
    # Return stored data as JSON
    stored_data = get_stored_data()
    return jsonify(stored_data)

@app.route("/download")
def download():
    # Generate a JSON file for download
    stored_data = get_stored_data()

    # Get the directory of the current script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    json_file_path = os.path.join(script_dir, "data.json")

    # Write the JSON data to the file
    with open(json_file_path, "w") as f:
        json.dump(stored_data, f)

    # Use Flask's `send_file` to send the file for download
    return send_file(json_file_path, as_attachment=True)

if __name__ == "__main__":
    app.run(debug=True)