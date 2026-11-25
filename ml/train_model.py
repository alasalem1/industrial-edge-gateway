import csv
import numpy as np
from pathlib import Path
from sklearn.ensemble import IsolationForest
from sklearn.metrics import confusion_matrix, classification_report

data = np.genfromtxt(
    "ml/datasets/normal_operation.csv",
    delimiter=",",
    skip_header=1
)

rng = np.random.default_rng(123)
normal_test = data[rng.choice(len(data), 300, replace=False)]

# Abnormal operating conditions: individual faults and unusual combinations.
anomalies = np.array([
    [75.0, 5.0, 1.5, 1250.0],
    [42.0, 8.2, 1.5, 1250.0],
    [42.0, 5.0, 5.2, 1250.0],
    [42.0, 5.0, 1.5, 1850.0],
    [58.0, 6.6, 3.5, 1600.0],
    [36.0, 5.8, 2.1, 1500.0],
    [52.0, 4.0, 0.6, 900.0],
    [55.0, 6.8, 3.8, 1700.0],
] * 25)

X_test = np.vstack((normal_test, anomalies))
y_test = np.array([1] * len(normal_test) + [-1] * len(anomalies))

model = IsolationForest(
    n_estimators=64,
    max_samples=256,
    contamination=0.03,
    random_state=42,
    n_jobs=1
)

model.fit(data)
predictions = model.predict(X_test)

print("===== ISOLATION FOREST EVALUATION =====")
print("Training samples:", len(data))
print("Trees:", len(model.estimators_))
print("Samples per tree:", model.max_samples_)
print("Model offset:", model.offset_)
print()
print(classification_report(
    y_test,
    predictions,
    labels=[1, -1],
    target_names=["Normal", "Anomaly"],
    zero_division=0
))
print("Confusion matrix (normal, anomaly):")
print(confusion_matrix(y_test, predictions, labels=[1, -1]))

print("\n===== INDIVIDUAL ANOMALY RESULTS =====")
for sample in anomalies[:8]:
    prediction = model.predict(sample.reshape(1, -1))[0]
    score = model.decision_function(sample.reshape(1, -1))[0]
    print(
        f"T={sample[0]:5.1f} C  P={sample[1]:4.1f} bar  "
        f"V={sample[2]:4.1f} mm/s  RPM={sample[3]:4.0f}  "
        f"prediction={'ANOMALY' if prediction == -1 else 'NORMAL'}  "
        f"decision={score:.4f}"
    )

Path("ml/isolation_forest.joblib").parent.mkdir(exist_ok=True)
import joblib
joblib.dump(model, "ml/isolation_forest.joblib")
print("\nSaved model: ml/isolation_forest.joblib")
