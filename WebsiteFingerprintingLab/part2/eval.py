import json
import numpy as np

from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder

def eval():
    y_pred_full, y_test_full = [], []

    # Load once (same data reused across 10 shuffles)
    with open("traces.out", "r") as f:
        payload = json.load(f)

    X = np.array(payload["traces"], dtype=np.float32)   # shape (N, F)
    y_raw = np.array(payload["labels"])                 # shape (N,)

    # Encode string labels to integers
    le = LabelEncoder()
    y = le.fit_transform(y_raw)

    for i in range(10):
        # Stratified split preserves class ratios
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.3, random_state=i, stratify=y
        )

        clf = RandomForestClassifier(
            n_estimators=200, max_depth=None, n_jobs=-1, random_state=i
        )
        clf.fit(X_train, y_train)

        y_pred = clf.predict(X_test)

        # accumulate for a single overall report
        y_test_full.extend(y_test.tolist())
        y_pred_full.extend(y_pred.tolist())

    # Print report with original label names
    target_names = le.inverse_transform(sorted(set(y)))
    print(classification_report(y_test_full, y_pred_full, target_names=target_names))

if __name__ == "__main__":
    eval()
