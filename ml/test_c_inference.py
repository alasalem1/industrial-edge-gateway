import ctypes
import os
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

model = IsolationForest(
    n_estimators=128,
    max_samples=256,
    contamination="auto",
    random_state=42,
    n_jobs=1
)
model.fit(train)

threshold = float(np.quantile(model.score_samples(holdout), 0.01))

# Recreate the independent fault scenarios.
def normal_samples(n):
    load = rng.uniform(0.05, 0.95, n)
    return np.column_stack([
        38 + 12*load + rng.normal(0, 1.2, n),
        4.2 + 1.5*load + rng.normal(0, 0.12, n),
        0.8 + 1.1*load + rng.normal(0, 0.12, n),
        900 + 600*load + rng.normal(0, 30, n)
    ])

faults = []
for feature, low, high in [
    (0, 68, 85), (1, 7.8, 9.0),
    (2, 4.8, 7.0), (3, 1800, 2200)
]:
    samples = normal_samples(200)
    samples[:, feature] = rng.uniform(low, high, 200)
    faults.append(samples)

faults.append(np.column_stack([
    rng.uniform(53, 65, 200),
    rng.uniform(3.8, 4.5, 200),
    rng.uniform(3.0, 4.5, 200),
    rng.uniform(850, 1050, 200)
]))

X = np.vstack([holdout] + faults)

library_path = os.environ.get(
    "SENTINEL_AI_LIBRARY",
    "/tmp/libsentinel_ai.so"
)
lib = ctypes.CDLL(library_path)
predict = lib.sentinel_ai_predict
predict.argtypes = [
    ctypes.POINTER(ctypes.c_double),
    ctypes.POINTER(ctypes.c_double)
]
predict.restype = ctypes.c_int

c_scores = []
c_labels = []

for sample in X:
    values = (ctypes.c_double * 4)(*sample)
    score = ctypes.c_double()
    label = predict(values, ctypes.byref(score))
    if label < 0:
        raise RuntimeError("C inference returned an error")
    c_scores.append(score.value)
    c_labels.append(label)

c_scores = np.array(c_scores)
c_labels = np.array(c_labels)
py_scores = model.score_samples(X)
py_labels = (py_scores < threshold).astype(int)

differences = np.abs(c_scores - py_scores)

print("===== C / SCIKIT-LEARN PARITY TEST =====")
print("Test samples:", len(X))
print("Maximum score difference:", f"{differences.max():.12g}")
print("Mean score difference:", f"{differences.mean():.12g}")
print("Classification mismatches:", np.sum(c_labels != py_labels))
print("Calibrated threshold:", f"{threshold:.9f}")

print("\n===== RESULT =====")
if differences.max() < 1e-6 and np.array_equal(c_labels, py_labels):
    print("PASS: C inference matches scikit-learn.")
else:
    print("FAIL: C and scikit-learn results differ.")
    raise SystemExit(1)
