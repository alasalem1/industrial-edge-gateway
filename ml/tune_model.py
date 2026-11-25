import numpy as np
from sklearn.ensemble import IsolationForest

data = np.genfromtxt(
    "ml/datasets/normal_operation.csv",
    delimiter=",", skip_header=1
)

rng = np.random.default_rng(2026)
normal = data[rng.choice(len(data), 500, replace=False)]

def baseline(n):
    load = rng.uniform(0.1, 0.9, n)
    return np.column_stack([
        38 + 12*load + rng.normal(0, 1.2, n),
        4.2 + 1.5*load + rng.normal(0, 0.12, n),
        0.8 + 1.1*load + rng.normal(0, 0.12, n),
        900 + 600*load + rng.normal(0, 30, n)
    ])

faults = []
for feature, low, high in [
    (0, 68, 85),
    (1, 7.8, 9.0),
    (2, 4.8, 7.0),
    (3, 1800, 2200),
]:
    samples = baseline(200)
    samples[:, feature] = rng.uniform(low, high, 200)
    faults.append(samples)

# Unusual combinations of individually less-extreme readings.
mixed = np.column_stack([
    rng.uniform(53, 65, 200),
    rng.uniform(3.8, 4.5, 200),
    rng.uniform(3.0, 4.5, 200),
    rng.uniform(850, 1050, 200)
])
faults.append(mixed)

anomalies = np.vstack(faults)
X = np.vstack([normal, anomalies])
y = np.array([1]*len(normal) + [-1]*len(anomalies))

print("Normal test samples:", len(normal))
print("Anomaly test samples:", len(anomalies))
print("\nContamination comparison")

for contamination in [0.01, 0.02, 0.03]:
    model = IsolationForest(
        n_estimators=128,
        max_samples=256,
        contamination=contamination,
        random_state=42,
        n_jobs=1
    )
    model.fit(data)
    pred = model.predict(X)

    false_alarms = np.sum(pred[:len(normal)] == -1)
    detected = np.sum(pred[len(normal):] == -1)

    print(f"\nContamination: {contamination:.0%}")
    print(f"Normal false alarms: {false_alarms}/{len(normal)}")
    print(f"Anomalies detected: {detected}/{len(anomalies)}")

    for i, name in enumerate([
        "Temperature", "Pressure", "Vibration",
        "Motor RPM", "Mixed patterns"
    ]):
        group = pred[len(normal)+i*200:len(normal)+(i+1)*200]
        print(f"  {name:16s}: {np.sum(group == -1)}/200")
