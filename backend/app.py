from flask import Flask, render_template, jsonify, request
from data_source import read_data, write_data

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("dashboard.html")

@app.route("/data")
def data():
    return jsonify(read_data())

@app.route("/ingest", methods=["POST"])
def ingest():
    # Exemplo de ingestão: POST JSON para atualizar a base
    payload = request.get_json(force=True)
    write_data(payload)
    return {"status": "ok"}

if __name__ == "__main__":
    # Use 0.0.0.0 para expor na rede local; mude a porta se necessário
    app.run(host="0.0.0.0", port=5000)
