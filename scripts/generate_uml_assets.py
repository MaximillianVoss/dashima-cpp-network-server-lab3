from __future__ import annotations

import math
from dataclasses import dataclass
from html import escape
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "docs" / "uml"
PNG_PATH = OUT_DIR / "class_diagram.png"
SVG_PATH = OUT_DIR / "class_diagram.svg"


@dataclass(frozen=True)
class ClassBox:
    name: str
    x: int
    y: int
    w: int
    h: int
    stereotype: str
    fields: tuple[str, ...]
    methods: tuple[str, ...]

    def anchor(self, side: str, offset: int = 0) -> tuple[int, int]:
        if side == "top":
            return self.x + self.w // 2 + offset, self.y
        if side == "bottom":
            return self.x + self.w // 2 + offset, self.y + self.h
        if side == "left":
            return self.x, self.y + self.h // 2 + offset
        if side == "right":
            return self.x + self.w, self.y + self.h // 2 + offset
        raise ValueError(f"Unknown anchor side: {side}")


@dataclass(frozen=True)
class Relation:
    source: str
    target: str
    kind: str
    points: tuple[tuple[int, int], ...]


BOXES = {
    "Packet": ClassBox(
        "Packet",
        640,
        190,
        330,
        215,
        "abstract",
        ("- senderAddress_: string", "- recipientAddress_: string", "- message_: MessageDescriptor"),
        ("+ type(): PacketType", "+ describe(): string", "+ senderAddress(): string"),
    ),
    "MessageDescriptor": ClassBox(
        "MessageDescriptor",
        120,
        210,
        320,
        145,
        "model",
        ("- content_: string",),
        ("+ length(): size_t", "+ content(): string"),
    ),
    "LinkDescriptor": ClassBox(
        "LinkDescriptor",
        1160,
        210,
        320,
        145,
        "model",
        ("- protocol_: ProtocolType", "- serverName_: string"),
        ("+ describe(): string",),
    ),
    "MailPacket": ClassBox(
        "MailPacket",
        110,
        560,
        310,
        165,
        "model",
        ("- userName_: string",),
        ("+ toFilePacket(): FilePacket", "+ describe(): string"),
    ),
    "FilePacket": ClassBox(
        "FilePacket",
        645,
        560,
        320,
        175,
        "model",
        ("- codeType_: CodeType", "- infoType_: InfoType"),
        ("+ toHypertextPacket(): HypertextPacket", "+ codeType(): CodeType"),
    ),
    "HypertextPacket": ClassBox(
        "HypertextPacket",
        1170,
        560,
        350,
        195,
        "model",
        ("- codeType_: CodeType", "- infoType_: InfoType", "- links_: vector<LinkDescriptor>"),
        ("+ linkCount(): size_t", "+ links(): vector<LinkDescriptor>"),
    ),
    "HashTable": ClassBox(
        "HashTable<Key, Value>",
        80,
        880,
        380,
        180,
        "template container",
        ("- buckets_: unique_ptr<Bucket[]>", "- size_: size_t"),
        ("+ insert(key, value): bool", "+ erase(key): bool", "+ begin()/end(): iterator"),
    ),
    "TransmissionTable": ClassBox(
        "TransmissionTable",
        610,
        880,
        410,
        185,
        "model",
        ("- table_: HashTable<PacketKey, shared_ptr<Packet>>",),
        ("+ insert(packet): bool", "+ find(recipient, type): Packet", "+ remove(recipient, type): bool"),
    ),
    "ServerDescriptor": ClassBox(
        "ServerDescriptor",
        1160,
        875,
        400,
        215,
        "model",
        ("- networkName_: string", "- networkAddress_: string", "- transmissions_: TransmissionTable"),
        ("+ addPacket(packet): bool", "+ selectByRecipientPriority()", "+ percentagesParallel()"),
    ),
    "ConsoleView": ClassBox(
        "ConsoleView",
        120,
        1250,
        350,
        175,
        "view",
        ("- input_: istream", "- output_: ostream"),
        ("+ showMenu()", "+ readPacketType()", "+ showPercentages()"),
    ),
    "ServerController": ClassBox(
        "ServerController",
        640,
        1240,
        390,
        195,
        "controller",
        ("- server_: ServerDescriptor", "- view_: ConsoleView"),
        ("+ run()", "- addPacket()", "- convertPacket()"),
    ),
    "PacketStatistics": ClassBox(
        "PacketStatistics",
        1170,
        1250,
        390,
        135,
        "service",
        (),
        ("+ calculateSequential(table)", "+ calculateParallel(table)"),
    ),
}


def p(name: str, side: str, offset: int = 0) -> tuple[int, int]:
    return BOXES[name].anchor(side, offset)


RELATIONS = [
    # Generalization: arrowhead is on the base class side.
    Relation("MailPacket", "Packet", "inheritance", (p("MailPacket", "top"), (735, 455), p("Packet", "bottom", -70))),
    Relation("FilePacket", "Packet", "inheritance", (p("FilePacket", "top"), p("Packet", "bottom"))),
    Relation("HypertextPacket", "Packet", "inheritance", (p("HypertextPacket", "top"), (875, 455), p("Packet", "bottom", 70))),
    # Composition: filled diamond is on the owner/whole side.
    Relation("Packet", "MessageDescriptor", "composition", (p("Packet", "left", -35), p("MessageDescriptor", "right", -10))),
    Relation("HypertextPacket", "LinkDescriptor", "composition", (p("HypertextPacket", "top"), p("LinkDescriptor", "bottom"))),
    Relation("TransmissionTable", "HashTable", "composition", (p("TransmissionTable", "left"), p("HashTable", "right"))),
    Relation("ServerDescriptor", "TransmissionTable", "composition", (p("ServerDescriptor", "left", -25), p("TransmissionTable", "right", -15))),
    Relation("ServerController", "ServerDescriptor", "composition", (p("ServerController", "top", 90), (1335, 1160), p("ServerDescriptor", "bottom", 60))),
    Relation("ServerController", "ConsoleView", "composition", (p("ServerController", "left", 18), p("ConsoleView", "right", 18))),
    # Dependencies: dashed arrow points to the used class.
    Relation("PacketStatistics", "TransmissionTable", "dependency", (p("PacketStatistics", "left", -35), (930, 1165), p("TransmissionTable", "bottom", 80))),
    Relation("MailPacket", "FilePacket", "dependency", (p("MailPacket", "right", -10), p("FilePacket", "left", -10))),
    Relation("FilePacket", "HypertextPacket", "dependency", (p("FilePacket", "right", -10), p("HypertextPacket", "left", -10))),
]


def load_font(size: int, bold: bool = False) -> ImageFont.FreeTypeFont:
    candidates = [
        Path("C:/Windows/Fonts/arialbd.ttf" if bold else "C:/Windows/Fonts/arial.ttf"),
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
    ]
    for candidate in candidates:
        if candidate.exists():
            return ImageFont.truetype(str(candidate), size)
    return ImageFont.load_default()


def draw_polyline(draw: ImageDraw.ImageDraw, points: tuple[tuple[int, int], ...], color: str, dashed: bool = False) -> None:
    for start, end in zip(points, points[1:]):
        if dashed:
            draw_dashed_segment(draw, start, end, color)
        else:
            draw.line([start, end], fill=color, width=3)


def draw_dashed_segment(draw: ImageDraw.ImageDraw, start: tuple[int, int], end: tuple[int, int], color: str) -> None:
    x1, y1 = start
    x2, y2 = end
    length = max(1.0, math.hypot(x2 - x1, y2 - y1))
    dash = 14.0
    gap = 9.0
    distance = 0.0
    while distance < length:
        segment_end = min(length, distance + dash)
        if segment_end > distance:
            a = distance / length
            b = segment_end / length
            draw.line(
                [(x1 + (x2 - x1) * a, y1 + (y2 - y1) * a), (x1 + (x2 - x1) * b, y1 + (y2 - y1) * b)],
                fill=color,
                width=3,
            )
        distance += dash + gap


def unit_vector(start: tuple[int, int], end: tuple[int, int]) -> tuple[float, float]:
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    length = max(1.0, math.hypot(dx, dy))
    return dx / length, dy / length


def perpendicular(vector: tuple[float, float]) -> tuple[float, float]:
    return -vector[1], vector[0]


def draw_open_triangle(draw: ImageDraw.ImageDraw, before: tuple[int, int], tip: tuple[int, int], color: str) -> None:
    ux, uy = unit_vector(before, tip)
    px, py = perpendicular((ux, uy))
    size = 18
    base_x = tip[0] - ux * size
    base_y = tip[1] - uy * size
    points = [
        tip,
        (int(base_x + px * 11), int(base_y + py * 11)),
        (int(base_x - px * 11), int(base_y - py * 11)),
    ]
    draw.polygon(points, outline=color, fill="#ffffff")


def draw_dependency_arrow(draw: ImageDraw.ImageDraw, before: tuple[int, int], tip: tuple[int, int], color: str) -> None:
    ux, uy = unit_vector(before, tip)
    px, py = perpendicular((ux, uy))
    size = 15
    base_x = tip[0] - ux * size
    base_y = tip[1] - uy * size
    draw.polygon(
        [
            tip,
            (int(base_x + px * 7), int(base_y + py * 7)),
            (int(base_x - px * 7), int(base_y - py * 7)),
        ],
        fill=color,
    )


def draw_diamond(draw: ImageDraw.ImageDraw, tip: tuple[int, int], after: tuple[int, int], color: str) -> None:
    ux, uy = unit_vector(tip, after)
    px, py = perpendicular((ux, uy))
    length = 18
    half_width = 10
    center_x = tip[0] + ux * length
    center_y = tip[1] + uy * length
    far_x = tip[0] + ux * length * 2
    far_y = tip[1] + uy * length * 2
    points = [
        tip,
        (int(center_x + px * half_width), int(center_y + py * half_width)),
        (int(far_x), int(far_y)),
        (int(center_x - px * half_width), int(center_y - py * half_width)),
    ]
    draw.polygon(points, outline=color, fill=color)


def draw_relation(draw: ImageDraw.ImageDraw, relation: Relation) -> None:
    color = "#4a5568"
    draw_polyline(draw, relation.points, color, dashed=relation.kind == "dependency")
    if relation.kind == "inheritance":
        draw_open_triangle(draw, relation.points[-2], relation.points[-1], color)
    elif relation.kind == "dependency":
        draw_dependency_arrow(draw, relation.points[-2], relation.points[-1], color)
    elif relation.kind == "composition":
        draw_diamond(draw, relation.points[0], relation.points[1], color)


def draw_box(draw: ImageDraw.ImageDraw, box: ClassBox, title_font: ImageFont.FreeTypeFont, text_font: ImageFont.FreeTypeFont) -> None:
    draw.rounded_rectangle([box.x, box.y, box.x + box.w, box.y + box.h], radius=12, outline="#1f2937", width=2, fill="#ffffff")
    draw.rounded_rectangle([box.x, box.y, box.x + box.w, box.y + 48], radius=12, outline="#1f2937", width=2, fill="#e8eef5")
    draw.text((box.x + 14, box.y + 8), box.name, font=title_font, fill="#0b2545")
    draw.text((box.x + 14, box.y + 30), f"<<{box.stereotype}>>", font=text_font, fill="#4b5563")
    draw.line([(box.x, box.y + 50), (box.x + box.w, box.y + 50)], fill="#1f2937", width=2)
    y = box.y + 60
    for line in box.fields:
        draw.text((box.x + 14, y), line, font=text_font, fill="#111827")
        y += 22
    draw.line([(box.x, y + 4), (box.x + box.w, y + 4)], fill="#9ca3af", width=1)
    y += 12
    for line in box.methods:
        draw.text((box.x + 14, y), line, font=text_font, fill="#111827")
        y += 22


def generate_png() -> None:
    image = Image.new("RGB", (1680, 1520), "#ffffff")
    draw = ImageDraw.Draw(image)
    title_font = load_font(22, bold=True)
    box_title_font = load_font(17, bold=True)
    text_font = load_font(14)

    draw.text((535, 28), "UML class diagram: network server", font=title_font, fill="#0b2545")
    for relation in RELATIONS:
        draw_relation(draw, relation)
    for box in BOXES.values():
        draw_box(draw, box, box_title_font, text_font)
    image.save(PNG_PATH)


def svg_text(x: int, y: int, text: str, size: int = 14, weight: str = "normal", fill: str = "#111827") -> str:
    return f'<text x="{x}" y="{y}" font-family="Arial, sans-serif" font-size="{size}" font-weight="{weight}" fill="{fill}">{escape(text)}</text>'


def svg_points(points: tuple[tuple[int, int], ...]) -> str:
    return " ".join(f"{x},{y}" for x, y in points)


def svg_polyline(points: tuple[tuple[int, int], ...], dashed: bool = False) -> str:
    dash = ' stroke-dasharray="14 9"' if dashed else ""
    return f'<polyline points="{svg_points(points)}" fill="none" stroke="#4a5568" stroke-width="3"{dash}/>'


def polygon_points(points: list[tuple[int, int]]) -> str:
    return " ".join(f"{x},{y}" for x, y in points)


def triangle_points(before: tuple[int, int], tip: tuple[int, int]) -> list[tuple[int, int]]:
    ux, uy = unit_vector(before, tip)
    px, py = perpendicular((ux, uy))
    size = 18
    base_x = tip[0] - ux * size
    base_y = tip[1] - uy * size
    return [
        tip,
        (int(base_x + px * 11), int(base_y + py * 11)),
        (int(base_x - px * 11), int(base_y - py * 11)),
    ]


def dependency_arrow_points(before: tuple[int, int], tip: tuple[int, int]) -> list[tuple[int, int]]:
    ux, uy = unit_vector(before, tip)
    px, py = perpendicular((ux, uy))
    size = 15
    base_x = tip[0] - ux * size
    base_y = tip[1] - uy * size
    return [
        tip,
        (int(base_x + px * 7), int(base_y + py * 7)),
        (int(base_x - px * 7), int(base_y - py * 7)),
    ]


def diamond_points(tip: tuple[int, int], after: tuple[int, int]) -> list[tuple[int, int]]:
    ux, uy = unit_vector(tip, after)
    px, py = perpendicular((ux, uy))
    length = 18
    half_width = 10
    center_x = tip[0] + ux * length
    center_y = tip[1] + uy * length
    far_x = tip[0] + ux * length * 2
    far_y = tip[1] + uy * length * 2
    return [
        tip,
        (int(center_x + px * half_width), int(center_y + py * half_width)),
        (int(far_x), int(far_y)),
        (int(center_x - px * half_width), int(center_y - py * half_width)),
    ]


def svg_relation(relation: Relation) -> str:
    parts = [svg_polyline(relation.points, dashed=relation.kind == "dependency")]
    if relation.kind == "inheritance":
        parts.append(f'<polygon points="{polygon_points(triangle_points(relation.points[-2], relation.points[-1]))}" fill="#ffffff" stroke="#4a5568" stroke-width="2"/>')
    elif relation.kind == "dependency":
        parts.append(f'<polygon points="{polygon_points(dependency_arrow_points(relation.points[-2], relation.points[-1]))}" fill="#4a5568"/>')
    elif relation.kind == "composition":
        parts.append(f'<polygon points="{polygon_points(diamond_points(relation.points[0], relation.points[1]))}" fill="#4a5568" stroke="#4a5568" stroke-width="2"/>')
    return "\n".join(parts)


def generate_svg() -> None:
    parts = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="1680" height="1520" viewBox="0 0 1680 1520">',
        '<rect width="1680" height="1520" fill="white"/>',
        svg_text(535, 48, "UML class diagram: network server", 22, "700", "#0b2545"),
    ]
    for relation in RELATIONS:
        parts.append(svg_relation(relation))
    for box in BOXES.values():
        parts.append(f'<rect x="{box.x}" y="{box.y}" width="{box.w}" height="{box.h}" rx="12" fill="#ffffff" stroke="#1f2937" stroke-width="2"/>')
        parts.append(f'<rect x="{box.x}" y="{box.y}" width="{box.w}" height="50" rx="12" fill="#e8eef5" stroke="#1f2937" stroke-width="2"/>')
        parts.append(svg_text(box.x + 14, box.y + 22, box.name, 17, "700", "#0b2545"))
        parts.append(svg_text(box.x + 14, box.y + 42, f"<<{box.stereotype}>>", 13, "400", "#4b5563"))
        parts.append(f'<line x1="{box.x}" y1="{box.y + 50}" x2="{box.x + box.w}" y2="{box.y + 50}" stroke="#1f2937" stroke-width="2"/>')
        y = box.y + 76
        for line in box.fields:
            parts.append(svg_text(box.x + 14, y, line, 14))
            y += 22
        parts.append(f'<line x1="{box.x}" y1="{y + 4}" x2="{box.x + box.w}" y2="{y + 4}" stroke="#9ca3af" stroke-width="1"/>')
        y += 26
        for line in box.methods:
            parts.append(svg_text(box.x + 14, y, line, 14))
            y += 22
    parts.append("</svg>")
    SVG_PATH.write_text("\n".join(parts), encoding="utf-8", newline="\n")


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    generate_png()
    generate_svg()


if __name__ == "__main__":
    main()
