import subprocess
import sys
from pathlib import Path

def test_default_message():
    script = Path(__file__).parent.parent / 'main.py'
    result = subprocess.run(
        [sys.executable, str(script)], capture_output=True, text=True
    )
    assert result.returncode == 0
    assert result.stdout.strip() == 'Hello from ExampleAgent!'

def test_custom_message():
    script = Path(__file__).parent.parent / 'main.py'
    custom = 'Test Message'
    result = subprocess.run(
        [sys.executable, str(script), '-m', custom], capture_output=True, text=True
    )
    assert result.returncode == 0
    assert result.stdout.strip() == custom
