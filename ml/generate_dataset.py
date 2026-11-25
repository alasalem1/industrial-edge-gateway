import csv
import numpy as np
from pathlib import Path

rng = np.random.default_rng(42)
count = 3000

load = rng.uniform(0.05, 0.95, count)

temperature = 38 + 12 * load + rng.normal(0, 1.2, count)
pressure = 4.2 + 1.5 * load + rng.normal(0, 0.12, count)
vibration = 0.8 + 1.1 * load + rng.normal(0, 0.12, count)
motor_rpm = 900 + 600 * load + rng.normal(0, 30, count)

path = Path("ml/datasets/normal_operation.csv")

with path.open("w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow([
        "temperature_c",
        "pressure_bar",
        "vibration_mm_s",
        "motor_rpm"
    ])
    writer.writerows(zip(
        temperature, pressure, vibration, motor_rpm
    ))

print("Dataset:", path)
print("Samples:", count)
print("Features: temperature, pressure, vibration, motor RPM")
print("\nNormal operating statistics:")
for name, values in [
    ("Temperature (C)", temperature),
    ("Pressure (bar)", pressure),
    ("Vibration (mm/s)", vibration),
    ("Motor RPM", motor_rpm),
]:
    print(f"{name:20s} min={values.min():.2f} max={values.max():.2f} mean={values.mean():.2f}")
