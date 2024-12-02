from flask import Flask, request
import os

app = Flask(__name__)

UPLOAD_FOLDER = os.path.join(os.getcwd(), "Users")
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route("/upload", methods=["POST"])
def upload():
    if "image" in request.files:
        image_file = request.files["image"]
        image_path = os.path.join(UPLOAD_FOLDER, image_file.filename)

        try:
            image_file.save(image_path)
            return "Image uploaded successfully!", 200
        except Exception as e:
            print(f"Error saving image: {e}")
            return "Failed to save image.", 500

    return "No image found in the request.", 400

if __name__ == "__main__":
    app.run(debug=True)
