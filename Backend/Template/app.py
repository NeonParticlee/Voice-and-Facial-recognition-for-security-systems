from flask import Flask, request, jsonify
import os
"""
app = Flask(__name__)

# Directory to save the users' folders
BASE_DIR = "Users"

# Ensure the base directory exists
if not os.path.exists(BASE_DIR):
    os.makedirs(BASE_DIR)

@app.route("/upload", methods=["POST"])
def upload():
    user_name = request.form.get("userName")
    if not user_name:
        return jsonify({"error": "User name is required"}), 400

    # Convert the user name to lowercase and create a folder
    folder_name = user_name.lower()
    user_folder = os.path.join(BASE_DIR, folder_name)
    if not os.path.exists(user_folder):
        os.makedirs(user_folder)

    # Save the uploaded file
    uploaded_file = request.files.get("image")
    if uploaded_file:
        # Get the count of files in the user's folder to generate a unique name
        file_count = len([f for f in os.listdir(user_folder) if os.path.isfile(os.path.join(user_folder, f))])
        file_name = f"{file_count + 1}.jpg"
        uploaded_file.save(os.path.join(user_folder, file_name))
        return jsonify({"message": f"File {file_name} saved successfully in {folder_name} folder"}), 200

    return jsonify({"error": "No file received"}), 400

if __name__ == "__main__":
    app.run(debug=True)


"""


