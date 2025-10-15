import json
from pathlib import Path

DATA_FILE = Path(__file__).parent / "data.json"

def read_data():
    if not DATA_FILE.exists():
        return {"tempo": [], "umidade": [], "temperatura": []}
    with open(DATA_FILE, "r") as f:
        return json.load(f)

def write_data(payload):
    with open(DATA_FILE, "w") as f:
        json.dump(payload, f)