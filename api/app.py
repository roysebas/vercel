from flask import Flask, render_template, jsonify
import firebase_admin
from firebase_admin import credentials, db

app = Flask(__name__)

# Inisialisasi Firebase Admin SDK
cred = credentials.Certificate("key.json")  # Path ke file SDK key Firebase
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://arduino-97ba9-default-rtdb.asia-southeast1.firebasedatabase.app/'  # Ganti dengan URL database realtime Firebase
})

# Referensi ke data sensor di Firebase
ref = db.reference('/WaterDistance')

@app.route('/')
def index():
    """Menampilkan halaman monitoring"""
    snapshot = ref.get()  # Ambil data terbaru dari Firebase
    distance = snapshot if snapshot else "Data belum tersedia"
    return render_template('index.html', distance=distance)

@app.route('/data')
def get_data():
    """API untuk mengambil data terbaru dalam format JSON"""
    snapshot = ref.get()
    data = {'distance': snapshot} if snapshot else {'distance': "Data belum tersedia"}
    return jsonify(data)

# Tidak perlu app.run() karena akan dijalankan oleh platform seperti Vercel

if __name__ == '__main__':
    app.run(debug=True, port=5000)