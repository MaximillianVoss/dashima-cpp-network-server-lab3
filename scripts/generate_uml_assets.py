from __future__ import annotations

from dataclasses import dataclass
from html import escape
from pathlib import Path
from typing import Iterable

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

    @property
    def center(self) -> tuple[int, int]:
        return self.x + self.w // 2, self.y + self.h // 2

    @property
    def top(self) -> tuple[int, int]:
        return self.x + self.w // 2, self.y

    @property
    def bottom(self) -> tuple[int, int]:
        return self.x + self.w // 2, self.y + self.h

    @property
    def left(self) -> tuple[int, int]:
        return self.x, self.y + self.h // 2

    @property
    def right(self) -> tuple[int, int]:
        return self.x + self.w, self.y + self.h // 2


BOXES = {
    "Packet": ClassBox(
        "Packet",
        640,
        230,
        320,
        215,
        "abstract",
        ("- senderAddress_: string", "- recipientAddress_: string", "- message_: MessageDescriptor"),
        ("+ type(): PacketType", "+ describe(): string", "+ senderAddress(): string"),
    ),
    "MailPacket": ClassBox(
        "MailPacket",
        180,
        560,
        300,
        165,
        "model",
        ("- userName_: string",),
        ("+ toFilePacket(): FilePacket", "+ describe(): string"),
    ),
    "FilePacket": ClassBox(
        "FilePacket",
        650,
        560,
        300,
        175,
        "model",
        ("- codeType_: CodeType", "- infoType_: InfoType"),
        ("+ toHypertextPacket(): HypertextPacket", "+ codeType(): CodeType"),
    ),
    "HypertextPacket": ClassBox(
        "HypertextPacket",
        1120,
        560,
        340,
        195,
        "model",
        ("- codeType_: CodeType", "- infoType_: InfoType", "- links_: vector<LinkDescriptor>"),
        ("+ linkCount(): size_t", "+ links(): vector<LinkDescriptor>"),
    ),
    "MessageDescriptor": ClassBox(
        "MessageDescriptor",
        170,
        235,
        300,
        145,
        "model",
        ("- content_: string",),
        ("+ length(): size_t", "+ content(): string"),
    ),
    "LinkDescriptor": ClassBox(
        "LinkDescriptor",
        1160,
        250,
        300,
        145,
        "model",
        ("- protocol_: ProtocolType", "- serverName_: string"),
        ("+ describe(): string",),
    ),
    "TransmissionTable": ClassBox(
        "TransmissionTable",
        580,
        850,
        390,
        185,
        "model",
        ("- table_: HashTable<PacketKey, shared_ptr<Packet>>",),
        ("+ insert(packet): bool", "+ find(recipient, type): Packet", "+ remove(recipient, type): bool"),
    ),
    "HashTable": ClassBox(
        "HashTable<Key, Value>",
        90,
        850,
        360,
        180,
        "template container",
        ("- buckets_: unique_ptr<Bucket[]>", "- size_: size_t"),
        ("+ insert(key, value): bool", "+ erase(key): bool", "+ begin()/end(): iterator"),
    ),
    "ServerDescriptor": ClassBox(
        "ServerDescriptor",
        1080,
        860,
        390,
        215,
        "model",
        ("- networkName_: string", "- networkAddress_: string", "- transmissions_: TransmissionTable"),
        ("+ addPacket(packet): bool", "+ selectByRecipientPriority()", "+ percentagesParallel()"),
    ),
    "PacketStatistics": ClassBox(
        "PacketStatistics",
        1080,
        1160,
        390,
        135,
        "service",
        (),
        ("+ calculateSequential(table)", "+ calculateParallel(table)"),
    ),
    "ConsoleView": ClassBox(
        "ConsoleView",
        190,
        1200,
        330,
        175,
        "view",
        ("- input_: istream", "- output_: ostream"),
        ("+ showMenu()", "+ readPacketType()", "+ showPercentages()"),
    ),
    "ServerController": ClassBox(
        "ServerController",
        650,
        1190,
        370,
        195,
        "controller",
        ("- server_: ServerDescriptor", "- view_: ConsoleView"),
        ("+ run()", "- addPacket()", "- convertPacket()"),
    ),
}


RELATIONS = [
    ("MailPacket", "Packet", "inherits"),
    ("FilePacket", "Packet", "inherits"),
    ("HypertextPacket", "Packet", "inherits"),
    ("Packet", "MessageDescriptor", "composition"),
    ("HypertextPacket", "LinkDescriptor", "composition"),
    ("TransmissionTable", "HashTable", "composition"),
    ("ServerDescriptor", "TransmissionTable", "composition"),
    ("ServerController", "ServerDescriptor", "composition"),
    ("ServerController", "ConsoleView", "composition"),
    ("PacketStatistics", "TransmissionTable", "dependency"),
    ("MailPacket", "FilePacket", "dependency"),
    ("FilePacket", "HypertextPacket", "dependency"),
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


def draw_arrow(draw: ImageDraw.ImageDraw, start: tuple[int, int], end: tuple[int, int], color: str, dashed: bool = False) -> None:
    if dashed:
        draw_dashed_line(draw, start, end, color)
    else:
        draw.line([start, end], fill=color, width=3)
    draw.polygon([(end[0], end[1]), (end[0] - 10, end[1] - 6), (end[0] - 6, end[1] + 10)], fill=color)


def draw_dashed_line(draw: ImageDraw.ImageDraw, start: tuple[int, int], end: tuple[int, int], color: str) -> None:
    x1, y1 = start
    x2, y2 = end
    steps = 24
    for i in range(steps):
        if i % 2 == 0:
            a = i / steps
            b = (i + 1) / steps
            draw.line(
                [(x1 + (x2 - x1) * a, y1 + (y2 - y1) * a), (x1 + (x2 - x1) * b, y1 + (y2 - y1) * b)],
                fill=color,
                width=3,
            )


def draw_relation(draw: ImageDraw.ImageDraw, source: ClassBox, target: ClassBox, kind: str) -> None:
    color = "#4a5568"
    if source.y > target.y:
        start = source.top
        end = target.bottom
    elif source.y < target.y:
        start = source.bottom
        end = target.top
    elif source.x < target.x:
        start = source.right
        end = target.left
    else:
        start = source.left
        end = target.right

    if kind == "inherits":
        draw.line([start, end], fill=color, width=3)
        x, y = end
        draw.polygon([(x, y), (x - 12, y + 18), (x + 12, y + 18)], outline=color, fill="#ffffff")
    elif kind == "composition":
        draw.line([start, end], fill=color, width=3)
        x, y = start
        draw.polygon([(x, y - 11), (x + 12, y), (x, y + 11), (x - 12, y)], outline=color, fill="#ffffff")
    else:
        draw_arrow(draw, start, end, color, dashed=True)


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
    image = Image.new("RGB", (1600, 1480), "#ffffff")
    draw = ImageDraw.Draw(image)
    title_font = load_font(22, bold=True)
    box_title_font = load_font(17, bold=True)
    text_font = load_font(14)

    draw.text((500, 28), "UML class diagram: network server", font=title_font, fill="#0b2545")
    for source_name, target_name, kind in RELATIONS:
        draw_relation(draw, BOXES[source_name], BOXES[target_name], kind)
    for box in BOXES.values():
        draw_box(draw, box, box_title_font, text_font)
    image.save(PNG_PATH)


def svg_text(x: int, y: int, text: str, size: int = 14, weight: str = "normal", fill: str = "#111827") -> str:
    return f'<text x="{x}" y="{y}" font-family="Arial, sans-serif" font-size="{size}" font-weight="{weight}" fill="{fill}">{escape(text)}</text>'


def svg_line(start: tuple[int, int], end: tuple[int, int], dashed: bool = False) -> str:
    dash = ' stroke-dasharray="10 8"' if dashed else ""
    return f'<line x1="{start[0]}" y1="{start[1]}" x2="{end[0]}" y2="{end[1]}" stroke="#4a5568" stroke-width="3"{dash}/>'


def relation_points(source: ClassBox, target: ClassBox) -> tuple[tuple[int, int], tuple[int, int]]:
    if source.y > target.y:
        return source.top, target.bottom
    if source.y < target.y:
        return source.bottom, target.top
    if source.x < target.x:
        return source.right, target.left
    return source.left, target.right


def generate_svg() -> None:
    parts = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="1600" height="1480" viewBox="0 0 1600 1480">',
        '<rect width="1600" height="1480" fill="white"/>',
        svg_text(500, 48, "UML class diagram: network server", 22, "700", "#0b2545"),
    ]
    for source_name, target_name, kind in RELATIONS:
        start, end = relation_points(BOXES[source_name], BOXES[target_name])
        parts.append(svg_line(start, end, dashed=kind == "dependency"))
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
