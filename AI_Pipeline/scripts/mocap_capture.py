#!/usr/bin/env python3
"""mocap_capture.py — etapa 4a: captura de movimento 100% local (MediaPipe).

Grava a webcam e extrai landmarks de pose (33 pontos, MediaPipe Pose) em JSON:
  work/<slug>/input/<slug>__mocap_{action}.json
  {"fps": 30, "frames": [{"t": float, "lm": [[x,y,z,conf]*33] }]}

Uso:
  mocap_capture.py --action walk --seconds 10 [--camera 0] [--slug qualquer]

Sem webcam (ou sem mediapipe) o script falha com explicação — a alternativa
para animações procedurais é a etapa 4b (mocap_retarget) com arquivo existente.
"""
import argparse
import json
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--action", required=True, help="walk|attack|idle|cast|custom")
    ap.add_argument("--seconds", type=float, default=CONFIG["mocap"]["seconds_per_action"])
    ap.add_argument("--camera", type=int, default=CONFIG["mocap"]["camera"])
    ap.add_argument("--slug", default="mocap", help="só para nomear o arquivo")
    args = ap.parse_args()

    try:
        import cv2
        import mediapipe as mp
    except ImportError:
        print("mocap: instale mediapipe e opencv (scripts/install.sh, passo MediaPipe).")
        return 2

    out_dir = ROOT / "work" / args.slug / "input"
    out_dir.mkdir(parents=True, exist_ok=True)
    out = out_dir / f"{args.slug}__mocap_{args.action}.json"

    cap = cv2.VideoCapture(args.camera)
    if not cap.isOpened():
        print("mocap: webcam não abriu (permissão de câmera no macOS?).")
        return 3

    with mp.solutions.pose.Pose(min_detection_confidence=0.6) as pose:
        frames = []
        fps = CONFIG["mocap"]["fps"]
        deadline = time.time() + args.seconds
        print(f"mocap: capturando '{args.action}' por {args.seconds}s — mova-se na frente da câmera.")
        while time.time() < deadline:
            ok, frame = cap.read()
            if not ok:
                break
            rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            res = pose.process(rgb)
            lm = []
            if res.pose_landmarks:
                for pl in res.pose_landmarks.landmark:
                    lm.append([round(pl.x, 5), round(pl.y, 5), round(pl.z, 5), round(pl.visibility, 3)])
            frames.append({"t": round(time.time() - deadline + args.seconds, 3), "lm": lm})
    cap.release()

    data = {"fps": fps, "frames": frames, "source": "mediapipe_pose", "action": args.action}
    out.write_text(json.dumps(data))
    good = sum(1 for f in frames if f["lm"])
    print(f"mocap: {len(frames)} frames, {good} com pose -> {out}")
    return 0 if good >= fps * 2 else 1


if __name__ == "__main__":
    sys.exit(main())
