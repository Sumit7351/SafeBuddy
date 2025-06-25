from flask import Flask, request, render_template, send_file
import os, subprocess, zipfile, shutil
from werkzeug.utils import secure_filename

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
RESULT_FOLDER = 'results'

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(RESULT_FOLDER, exist_ok=True)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/process', methods=['POST'])
def process_folder():
    zip_file = request.files['file']

    if not zip_file or not zip_file.filename.endswith('.zip'):
        return "Upload a ZIP file containing the folder.", 400

    filename = secure_filename(zip_file.filename)
    zip_path = os.path.join(UPLOAD_FOLDER, filename)
    extract_dir = os.path.join(UPLOAD_FOLDER, filename[:-4])

    # Save and unzip
    zip_file.save(zip_path)
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(extract_dir)

    # Run C++ binary on the folder
    try:
        subprocess.run(['./encrypt_decrypt', extract_dir], check=True)
    except subprocess.CalledProcessError as e:
        return f"Encryption/Decryption failed: {e}", 500

    # Zip the modified folder for download
    output_zip = os.path.join(RESULT_FOLDER, filename[:-4] + '_processed.zip')
    shutil.make_archive(output_zip[:-4], 'zip', extract_dir)

    return send_file(output_zip, as_attachment=True)

if __name__ == '__main__':
    app.run(debug=True)
