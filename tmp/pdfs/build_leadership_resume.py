from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import letter
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import inch
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, KeepTogether, PageBreak

OUT = "/Users/bmysoren/prep/output/pdf/bharath_mysore_leadership_resume.pdf"

styles = getSampleStyleSheet()
styles.add(ParagraphStyle(
    name="Name", parent=styles["Title"], fontName="Helvetica-Bold", fontSize=21,
    leading=25, alignment=TA_CENTER, textColor=colors.HexColor("#172033"), spaceAfter=3,
))
styles.add(ParagraphStyle(
    name="Contact", parent=styles["Normal"], fontName="Helvetica", fontSize=9.5,
    leading=13, alignment=TA_CENTER, textColor=colors.HexColor("#44546A"), spaceAfter=12,
))
styles.add(ParagraphStyle(
    name="Section", parent=styles["Heading2"], fontName="Helvetica-Bold", fontSize=11,
    leading=14, textColor=colors.HexColor("#1B5E8C"), spaceBefore=10, spaceAfter=4,
    borderWidth=0, borderPadding=0,
))
styles.add(ParagraphStyle(
    name="Body", parent=styles["Normal"], fontName="Helvetica", fontSize=9.3,
    leading=12.5, textColor=colors.HexColor("#1E293B"), spaceAfter=3,
))
styles.add(ParagraphStyle(
    name="Company", parent=styles["Normal"], fontName="Helvetica-Bold", fontSize=10.7,
    leading=13, textColor=colors.HexColor("#172033"), spaceAfter=1,
))
styles.add(ParagraphStyle(
    name="Role", parent=styles["Normal"], fontName="Helvetica-Bold", fontSize=9.3,
    leading=12, textColor=colors.HexColor("#334155"), spaceAfter=2,
))
styles.add(ParagraphStyle(
    name="ResumeBullet", parent=styles["Normal"], fontName="Helvetica", fontSize=9.2,
    leading=12.2, leftIndent=13, firstLineIndent=-9, bulletIndent=2,
    textColor=colors.HexColor("#1E293B"), spaceAfter=2.5,
))

def P(text, style="Body"):
    return Paragraph(text, styles[style])

def bullet(text):
    return Paragraph(text, styles["ResumeBullet"], bulletText="•")

def job(company, dates, group, bullets):
    content = [P(f"{company} <font color='#64748B'>| {dates}</font>", "Company"), P(group, "Role")]
    content.extend(bullet(x) for x in bullets)
    return KeepTogether(content + [Spacer(1, 5)])

doc = SimpleDocTemplate(
    OUT, pagesize=letter, rightMargin=0.62*inch, leftMargin=0.62*inch,
    topMargin=0.48*inch, bottomMargin=0.48*inch,
)
story = []
story += [P("BHARATH MYSORE", "Name"), P("Bothell, WA  |  +1 (425) 503-6839  |  bharathkumarmn@hotmail.com", "Contact")]

story += [P("TECHNICAL LEADERSHIP PROFILE", "Section")]
story += [P(
    "Senior technical leader with 20+ years of experience spanning cloud storage, distributed systems, "
    "kernel and driver development, hardware validation, and developer tools. Sets technical direction "
    "for ambiguous, cross-team initiatives; aligns platform investments to reliability, scale, and customer "
    "outcomes; and carries critical programs from prototype through durable release.",
)]

story += [P("LEADERSHIP STRENGTHS", "Section")]
story += [P(
    "Technical strategy and cross-team influence  •  Platform architecture and execution  •  "
    "Zero-to-one product delivery  •  Distributed storage and disaster recovery  •  "
    "Capacity efficiency and fault containment  •  Team formation and remote collaboration"
)]

story += [P("PROFESSIONAL EXPERIENCE", "Section")]
story.append(job("Oracle Corporation", "Jun 2016 - Present", "Cloud Storage Platforms | Unified Storage, Block Storage, Marketplace, and Data Pipeline", [
    "Set architecture and execution direction for core unified-storage capabilities serving object, block, and file workloads, including the replication layer shared across storage services.",
    "Drove cross-team platform initiatives spanning erasure coding for object storage, block-storage bring-up on the unified stack, migration from the legacy stack, and deep archival.",
    "Led cross-region replication and asynchronous volume-group replication from design through delivery, strengthening disaster-recovery capabilities for block-storage customers.",
    "Directed scale and availability improvements across the data path: expanded backup-backend capacity, introduced multi-process iSCSI serving, and re-architected networking to reduce hops and support more attached volumes.",
    "Led capacity-efficiency programs: cold-data compression reclaimed 20-30% of space; idle-volume backend optimization resulted in U.S. Patent 11,412,043; trimmed-block reclamation addressed 6-7% of unreferenced data.",
    "Raised platform resilience by refactoring the backend to contain failures by feature, volume, or device and automating failed-disk and host handling.",
    "Established and delivered a software-metering service from prototype to a public-facing, near-real-time Marketplace service.",
    "Helped redirect the team toward a data-processing service, contributing to the prototype and carrying the effort through internal release.",
]))
story.append(PageBreak())
story += [P("PROFESSIONAL EXPERIENCE (CONTINUED)", "Section")]

story.append(job("Microsoft Corporation", "Sep 2005 - May 2016", "Azure Storage", [
    "Designed and implemented a library supporting a proof-of-concept Azure Storage backend infrastructure using RDMA.",
]))
story.append(job("Microsoft Corporation", "Sep 2005 - May 2016", "Visual Studio", [
    "Led multiple debugger initiatives - Cluster Debugger for HPC/MPI, Parallel Debugger for concurrency, and GPU debugging for CUDA, OpenGL, and DirectX.",
    "Built and led a team of five local engineers and ten remote collaborators to deliver complex developer-tooling programs.",
]))
story.append(job("Microsoft Corporation", "Sep 2005 - May 2016", "HoloLens, Surface Hub/Kinect Tools, Xbox One", [
    "Owned end-to-end depth-camera functionality on HoloLens from prototype through release.",
    "Designed drivers and tools for hardware design verification, and developed a proof of concept for Xbox 360 graphics emulation on Xbox One.",
]))
story.append(job("Microsoft Corporation", "Sep 2005 - May 2016", "Global Technical Support Center", [
    "Served as a Windows OS, file-system, and disk subject-matter expert: led kernel-dump analysis, root-cause investigation, code reviews, and customer and engineering coordination through resolution.",
]))

story.append(job("Intel Technology India Private Limited", "Mar 2002 - Sep 2005", "Systems and Platform Software", [
    "Designed validation tools for CPU cores and cache/memory subsystems, including Linux kernel programming and drivers for test cards and chipsets.",
    "Drove complex hardware and OS issues to closure through debugging, root-cause analysis, and targeted patches; developed network-card management and configuration software for Windows and Linux.",
]))

story += [P("TECHNICAL FOUNDATION", "Section")]
story += [P(
    "Languages: C/C++, Java, Python, C#  |  Platforms: concurrent and parallel programming, "
    "gRPC, Folly, Boost, Terraform, Kubernetes, Docker, Helm, Prometheus, Apache Flink, "
    "ZooKeeper, iSCSI, Apache Kafka"
)]

def footer(canvas, doc):
    canvas.saveState()
    canvas.setStrokeColor(colors.HexColor("#CBD5E1"))
    canvas.setLineWidth(0.5)
    canvas.line(doc.leftMargin, 0.33*inch, letter[0]-doc.rightMargin, 0.33*inch)
    canvas.setFont("Helvetica", 7.5)
    canvas.setFillColor(colors.HexColor("#64748B"))
    canvas.drawRightString(letter[0]-doc.rightMargin, 0.19*inch, f"Bharath Mysore  |  {doc.page}")
    canvas.restoreState()

doc.build(story, onFirstPage=footer, onLaterPages=footer)
