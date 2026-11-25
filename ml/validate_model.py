import numpy as np
from sklearn.ensemble import IsolationForest

rng = np.random.default_rng(2027)
data = np.genfromtxt(
    "ml/datasets/normal_operation.csv",
    delimiter=",", skip_header=1
)

rng.shuffle(data)
split = int(len(data) * 0.8)
train, holdout = data[:split], data[split:]

def normal_samples(n):
    load = rng.uniform(0.05, 0.95, n)
    return np.column_stack([
        38 + 12*load + rng.normal(0, 1.2, n),
        4.2 + 1.5*load + rng.normal(0, 0.12, n),
        0.8 + 1.1*load + rng.normal(0, 0.12, n),
        900 + 600*load + rng.normal(0, 30, n)
    ])

def make_fault(feature, low, high, n=200):
    samples = normal_samples(n)
    samples[:, feature] = rng.uniform(low, high, n)
    return samples

fault_groups = [
    ("Temperature", make_fault(0, 68, 85)),
    ("Pressure", make_fault(1, 7.8, 9.0)),
    ("Vibration", make_fault(2, 4.8, 7.0)),
    ("Motor RPM", make_fault(3, 1800, 2200)),
]

mixed = np.column_stack([
    rng.uniform(53, 65, 200),
    rng.uniform(3.8, 4.5, 200),
    rng.uniform(3.0, 4.5, 200),
    rng.uniform(850, 1050, 200)
])
fault_groups.append(("Mixed patterns", mixed))

model = IsolationForest(
    n_estimators=128,
    max_samples=256,
    contamination="auto",
    random_state=42,
    n_jobs=1
)
model.fit(train)

normal_scores = model.score_samples(holdout)
fault_scores = [
    model.score_samples(samples)
    for _, samples in fault_groups
]

print("Training samples:", len(train))
print("Independent normal validation samples:", len(holdout))
print("\nThreshold calibration (lower score = more anomalous)")

for target in [0.005, 0.01, 0.02]:
    threshold = np.quantile(normal_scores, target)
    false_alarms = np.sum(normal_scores < threshold)

    print(f"\nTarget false-alarm rate: {target:.1%}")
    print(f"Measured normal false alarms: {false_alarms}/{len(holdout)}")
    print(f"Decision threshold: {threshold:.6f}")

    for (name, _), scores in zip(fault_groups, fault_scores):
        detected = np.sum(scores < threshold)
        print(f"  {name:16s}: {detected}/{len(scores)}")

print("\nValidation complete.")
