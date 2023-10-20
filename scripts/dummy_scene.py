import argparse
import random


class Transform:
    def __init__(self):
        self.trs = (0, 0, 0)
        self.rotation = (0, 0, 0)
        self.scale = (0, 0, 0)


def gen_uuid():
    return random.randint(0, 10000000000000000000)


def emit_mesh_static(file, name, parent, model, t: Transform):
    file.write(f' - name: "{name}"\n')
    file.write(f"   uuid: {gen_uuid()}\n")
    file.write(f"   parent: {parent}\n")
    file.write(f"   type: Object\n")
    file.write(f"   transform:\n")
    file.write(f"    translation: {t.trs[0]} {t.trs[1]} {t.trs[2]}\n")
    file.write(f"    rotation: {t.rotation[0]} {t.rotation[1]} {t.rotation[2]}\n")
    file.write(f"    scale: {t.scale[0]} {t.scale[1]} {t.scale[2]}\n")
    file.write(f"   properties:\n")
    file.write(f"    - class: ScenePropMeshStatic\n")
    file.write(f'      model: "{model}"\n')


def main():
    parser = argparse.ArgumentParser("generate test scene file")
    parser.add_argument(
        "--out", help="scene file name to save result", default="dummy.tree"
    )
    args = parser.parse_args()

    ts = []

    for i in range(-15, 15):
        for j in range(-15, 15):
            trf = Transform()
            trf.trs = (3 * i, 0, 3 * j)
            trf.scale = (0.1, 0.1, 0.1)
            trf.rotation = (
                random.random() * 180.0,
                random.random() * 180.0,
                random.random() * 180.0,
            )

            ts.append(trf)

    with open(args.out, "w") as file:
        for i, t in enumerate(ts):
            emit_mesh_static(
                file,
                name=f"suzanne_auto_{i}",
                parent="13626177115197044525",
                model="res://models/suzanne",
                t=t,
            )


if __name__ == "__main__":
    main()
