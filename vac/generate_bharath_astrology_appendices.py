#!/usr/bin/env python3
"""Regenerate the degree, Vimshottari, and event appendices in the profile."""

from __future__ import annotations

from datetime import datetime, timedelta, timezone
from pathlib import Path
from zoneinfo import ZoneInfo


PROFILE = Path(__file__).with_name("bharath_sidereal_astrology_profile.md")
BEGIN = "<!-- BEGIN GENERATED ASTROLOGY APPENDICES -->"
END = "<!-- END GENERATED ASTROLOGY APPENDICES -->"

SIGNS = [
    "Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
    "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces",
]

PLANETS = {
    "Ascendant": (10, 17 + 18 / 60),
    "Sun": (0, 23 + 38 / 60),
    "Moon": (8, 23 + 59 / 60),
    "Mars": (11, 14 + 20 / 60),
    "Mercury (R)": (0, 12 + 32 / 60),
    "Jupiter": (1, 13 + 46 / 60),
    "Venus": (11, 16 + 42 / 60),
    "Saturn": (3, 17 + 3 / 60),
    "Rahu": (5, 29 + 38 / 60),
    "Ketu": (11, 29 + 38 / 60),
}


def clamp_index(degree: float, width: float, count: int) -> int:
    return min(int(degree // width), count - 1)


def scaled_degree(degree: float, start: float, width: float) -> float:
    return (degree - start) / width * 30


def d1(sign: int, degree: float) -> tuple[int, float]:
    return sign, degree


def d2(sign: int, degree: float) -> tuple[int, float]:
    # Odd-numbered rāśis have Sun/Leo first and Moon/Cancer second;
    # even-numbered rāśis reverse the order.
    first = degree < 15
    odd_rashi = sign % 2 == 0
    target = 4 if (odd_rashi and first) or (not odd_rashi and not first) else 3
    return target, scaled_degree(degree, 0 if first else 15, 15)


def equal_varga(sign: int, degree: float, divisions: int, offsets: list[int]) -> tuple[int, float]:
    width = 30 / divisions
    index = clamp_index(degree, width, divisions)
    return (sign + offsets[index]) % 12, scaled_degree(degree, index * width, width)


def d3(sign: int, degree: float) -> tuple[int, float]:
    return equal_varga(sign, degree, 3, [0, 4, 8])


def d4(sign: int, degree: float) -> tuple[int, float]:
    return equal_varga(sign, degree, 4, [0, 3, 6, 9])


def d6(sign: int, degree: float) -> tuple[int, float]:
    width = 5
    index = clamp_index(degree, width, 6)
    start_sign = 0 if sign % 2 == 0 else 6
    return (start_sign + index) % 12, scaled_degree(degree, index * width, width)


def d9(sign: int, degree: float) -> tuple[int, float]:
    width = 30 / 9
    index = clamp_index(degree, width, 9)
    if sign in (0, 3, 6, 9):       # movable
        start_sign = sign
    elif sign in (1, 4, 7, 10):    # fixed
        start_sign = (sign + 8) % 12
    else:                           # dual
        start_sign = (sign + 4) % 12
    return (start_sign + index) % 12, scaled_degree(degree, index * width, width)


def d10(sign: int, degree: float) -> tuple[int, float]:
    width = 3
    index = clamp_index(degree, width, 10)
    start_sign = sign if sign % 2 == 0 else (sign + 8) % 12
    return (start_sign + index) % 12, scaled_degree(degree, index * width, width)


def d12(sign: int, degree: float) -> tuple[int, float]:
    return equal_varga(sign, degree, 12, list(range(12)))


def d30(sign: int, degree: float) -> tuple[int, float]:
    if sign % 2 == 0:  # odd-numbered rāśi
        segments = [(0, 5, 0), (5, 10, 10), (10, 18, 8), (18, 25, 2), (25, 30, 6)]
    else:
        segments = [(0, 5, 1), (5, 12, 5), (12, 20, 11), (20, 25, 9), (25, 30, 7)]
    for start, stop, target in segments:
        if degree < stop or stop == 30:
            return target, scaled_degree(degree, start, stop - start)
    raise AssertionError("unreachable")


VARGAS = {
    "D1 Rāśi": d1,
    "D2 Horā": d2,
    "D3 Drekkāṇa": d3,
    "D4 Chaturthāṁśa": d4,
    "D6 Ṣaṣṭhāṁśa": d6,
    "D9 Navāṁśa": d9,
    "D10 Daśāṁśa": d10,
    "D12 Dwādaśāṁśa": d12,
    "D30 Triṁśāṁśa": d30,
}


def format_degree(sign: int, degree: float) -> str:
    degree %= 30
    deg = int(degree)
    minute_float = (degree - deg) * 60
    minute = int(minute_float)
    second = round((minute_float - minute) * 60)
    if second == 60:
        second = 0
        minute += 1
    if minute == 60:
        minute = 0
        deg += 1
    return f"{SIGNS[sign]} {deg:02d}°{minute:02d}′{second:02d}″"


LORDS = ["Ketu", "Venus", "Sun", "Moon", "Mars", "Rahu", "Jupiter", "Saturn", "Mercury"]
YEARS = {"Ketu": 7, "Venus": 20, "Sun": 6, "Moon": 10, "Mars": 7,
         "Rahu": 18, "Jupiter": 16, "Saturn": 19, "Mercury": 17}
YEAR_DAYS = 365.2425
PACIFIC = ZoneInfo("America/Los_Angeles")


def period_sequence(start: datetime, parent_years: float, start_lord: str):
    index = LORDS.index(start_lord)
    cursor = start
    for offset in range(9):
        lord = LORDS[(index + offset) % 9]
        end = cursor + timedelta(days=parent_years * YEARS[lord] / 120 * YEAR_DAYS)
        yield lord, cursor, end
        cursor = end


def date_local(value: datetime) -> str:
    return value.astimezone(PACIFIC).strftime("%Y-%m-%d")


def build_dasha_data():
    birth_utc = datetime(1977, 5, 7, 20, 47, tzinfo=timezone.utc)
    remaining_fraction = ((26 + 40 / 60) - (23 + 59 / 60)) / (13 + 20 / 60)
    venus_end = birth_utc + timedelta(days=remaining_fraction * 20 * YEAR_DAYS)

    md_rows = [("Venus (balance)", birth_utc, venus_end)]
    cursor = venus_end
    md_order = ["Sun", "Moon", "Mars", "Rahu", "Jupiter", "Saturn", "Mercury", "Ketu", "Venus"]
    for lord in md_order:
        end = cursor + timedelta(days=YEARS[lord] * YEAR_DAYS)
        md_rows.append((lord, cursor, end))
        cursor = end

    ad_rows = []
    detailed_rows = []
    cutoff = datetime(2035, 1, 1, tzinfo=timezone.utc)
    for md, md_start, md_end in md_rows:
        if md == "Venus (balance)":
            continue
        md_years = YEARS[md]
        for ad, ad_start, ad_end in period_sequence(md_start, md_years, md):
            if ad_start >= cutoff:
                break
            if ad_end <= birth_utc:
                continue
            ad_rows.append((md, ad, ad_start, min(ad_end, cutoff)))
            # Detailed PD/SD is generated for the current Jupiter MD only.
            if md != "Jupiter":
                continue
            ad_years = md_years * YEARS[ad] / 120
            for pd, pd_start, pd_end in period_sequence(ad_start, ad_years, ad):
                if pd_start >= cutoff:
                    break
                pd_years = ad_years * YEARS[pd] / 120
                for sd, sd_start, sd_end in period_sequence(pd_start, pd_years, pd):
                    if sd_start >= cutoff:
                        break
                    if sd_end <= datetime(2022, 1, 1, tzinfo=timezone.utc):
                        continue
                    detailed_rows.append((md, ad, pd, sd, sd_start, min(sd_end, cutoff)))
    return md_rows, ad_rows, detailed_rows


EVENTS = [
    ("Mar 2002", "Joined Intel", "Income increased approximately 30%", "Mars–Ketu–Saturn"),
    ("Jan 2003", "Intel promotion", "First listed promotion", "Mars–Venus–Saturn"),
    ("Nov 2003–Jun 2004", "Intel U.S. assignment", "Temporary U.S. stay", "Mars–Moon"),
    ("Apr/May 2004", "Apartment gruhapravesh", "Property/home milestone", "Mars–Moon–Venus"),
    ("Sep 3, 2004", "Married Divya", "Marriage", "Rahu–Rahu–Rahu"),
    ("Oct 2005", "Joined Microsoft", "Income approximately doubled/tripled", "Rahu–Rahu–Mercury"),
    ("Dec 2006", "Father passed away", "Sensitive rectification anchor", "Rahu–Rahu–Mars"),
    ("Aug 2007", "Microsoft promotion 59→60", "Promotion", "Rahu–Jupiter–Saturn"),
    ("Jun 2008", "Moved to U.S. at level 61", "Promotion/relocation", "Rahu–Jupiter–Venus"),
    ("Nov 2008", "First son born", "Childbirth", "Rahu–Jupiter–Moon"),
    ("Sep 2009", "Mother passed away", "Sensitive rectification anchor", "Rahu–Saturn–Saturn"),
    ("Aug 2010", "Microsoft promotion 61→62", "Promotion", "Rahu–Saturn–Venus"),
    ("Jan 2013", "Second son born", "Childbirth", "Rahu–Mercury–Venus"),
    ("Feb 2014", "Moved into Bothell house", "Property/residence milestone", "Rahu–Mercury–Rahu"),
    ("Aug 2015", "Microsoft promotion 62→63", "Promotion", "Rahu–Ketu–Saturn"),
    ("Jun 2016", "Joined Oracle", "Income nearly doubled", "Rahu–Venus–Sun"),
    ("Jun 2016–Jul 2026", "Oracle tenure", "No promotion or major employer-assigned level increase", "Rahu→Jupiter MD"),
    ("May 2020", "Moved into second Bothell house", "Property/residence milestone", "Rahu–Moon–Jupiter"),
    ("2023", "Oracle W-2", "$552K", "Jupiter–Jupiter"),
    ("2024", "Oracle W-2", "$603K; increase mainly from stock appreciation", "Jupiter–Jupiter→Saturn"),
    ("2025", "Oracle W-2", "$728K; increase mainly from stock appreciation", "Jupiter–Saturn"),
    ("Oct 2025", "Oracle RSUs vested", "Referenced vest price was above $300 per share; vest-date documents and lot-level tax basis should be used for tax calculations", "Jupiter–Saturn–Venus"),
    ("Nov/Dec 2025", "Apple full loop", "Completed full loop", "Jupiter–Saturn–Sun/Moon"),
    ("Early 2026", "SBA loan approved for wife’s business", "$550K loan approved; current business lease runs through 2036", "Jupiter–Saturn"),
    ("Feb 2026", "Apple manager interview", "Did not materialize", "Jupiter–Saturn–Moon"),
    ("Mar 11, 2026 10:00–11:00 PDT", "Google screening 1", "Full-loop screening", "Jupiter–Saturn–Mars–Rahu"),
    ("Mar 11, 2026 12:00–12:45 PDT", "Google screening 2", "Full-loop screening", "Jupiter–Saturn–Mars–Rahu"),
    ("Apr 23, 2026 11:00–12:00 PDT", "Google design interview", "Staff loop", "Jupiter–Saturn–Mars near Sun/Moon micro-boundary"),
    ("Apr 28, 2026 10:15–11:45 PDT", "Google coding interview", "Near Mars→Rahu PD boundary", "Jupiter–Saturn–Mars/Rahu"),
    ("2026, after loop", "Google Staff approval", "Approved at Staff level; entered team matching", "Jupiter–Saturn–Rahu"),
    ("May 29, 2026", "Apple ASE manager outreach", "Staff Software Engineer, Storage Infrastructure", "Jupiter–Saturn–Rahu–Jupiter"),
    ("Jun 5, 2026 1:30 PM PDT", "Apple manager discussion", "ASE Storage Infrastructure", "Jupiter–Saturn–Rahu–Jupiter"),
    ("Jun 17, 2026 10:00–11:00 PDT", "Apple coding screen", "Second Apple 2026 loop", "Jupiter–Saturn–Rahu–Saturn"),
    ("Jun 24, 2026 1:10 PM PDT", "First Google team-match email", "Did not proceed: position required California location", "Jupiter–Saturn–Rahu–Saturn"),
    ("Jun 26, 2026 2:54–2:59 PM PDT", "Google Sunnyvale constraint and continued-search exchange", "Recruiter confirmed Sunnyvale was a hard requirement and recommended continuing the search for other teams. Bharath declined relocation, reaffirmed interest in Kirkland/Seattle infrastructure, cloud, or storage teams, and proposed syncing the following week about local options.", "Jupiter–Saturn–Rahu–Saturn"),
    ("Jul 1, 2026 1:05–2:05 PM PDT", "Apple design screen", "Second Apple 2026 loop", "Jupiter–Saturn–Rahu–Mercury"),
    ("Jul 8, 2026 2:51 PM PDT", "Apple rejection", "Other applicants selected for business-needs alignment; role treated as closed", "Jupiter–Saturn–Rahu–Mercury"),
    ("Jul 9, 2026", "Oracle RSU position reviewed", "Reported vested ORCL holdings more than 50% below the October 2025 vest value above $300 per share; exact basis remains subject to lot-level records", "Jupiter–Saturn–Rahu–Mercury"),
    ("Jul 10, 2026, approximately 9:00 PM PDT", "Google referral and prospective sponsorship follow-up", "Former colleague said his current contact had no L6 headcount but would keep watching. He also planned to contact a former Microsoft manager with whom Bharath worked from January through June 2016; Bharath is not currently in touch with that manager, now a Principal Engineer at Google. Both remain prospective referral/sponsorship paths, with no active team match identified.", "Jupiter–Saturn–Rahu–Mercury"),
    ("Jul 15, 2026 10:50 AM PDT", "Google recruiter follow-up sent", "Bharath followed up with Radhika regarding Staff/L6 team-match possibilities in Kirkland or Seattle and reiterated interest in storage platforms, distributed systems, cloud infrastructure, reliability, and systems architecture roles. Awaiting response; no active team match is implied by the outreach.", "Jupiter–Saturn–Rahu–Mercury"),
]


def build_appendix() -> str:
    lines = [BEGIN, "", "## 31. Derived Graha Degrees in All Corrected Charts", ""]
    lines.extend([
        "The following are mathematically derived varga longitudes from the supplied sidereal D1 degrees.",
        "For equal divisions, the longitude within the occupied division is proportionally expanded to 30°.",
        "For D2 and D30, the longitude is proportionally scaled within the applicable unequal/assigned segment.",
        "Some traditions use only varga signs rather than derived varga degrees; sign placement remains the primary interpretive factor.",
        "",
    ])

    for varga_name, calculator in VARGAS.items():
        values = {body: calculator(*position) for body, position in PLANETS.items()}
        asc_sign = values["Ascendant"][0]
        lines.extend([
            f"### 31.{list(VARGAS).index(varga_name) + 1} {varga_name}",
            "",
            "| Body | Derived longitude | House from varga ascendant |",
            "|---|---:|---:|",
        ])
        for body, (sign, degree) in values.items():
            house = (sign - asc_sign) % 12 + 1
            lines.append(f"| {body} | {format_degree(sign, degree)} | {house} |")
        lines.append("")

    md_rows, ad_rows, detailed_rows = build_dasha_data()
    lines.extend([
        "## 32. Vimshottari Daśā Schedule Through 2034",
        "",
        "> Dates are approximate and displayed as Pacific calendar dates. The supplied Moon longitude is rounded to one arcminute, so PD/SD boundaries can shift by several days. Do not treat these as hour-exact muhurta dates.",
        "",
        "### 32.1 Mahādaśā schedule",
        "",
        "| MD | Start | End |",
        "|---|---:|---:|",
    ])
    for md, start, end in md_rows:
        if start.year > 2034:
            break
        lines.append(f"| {md} | {date_local(start)} | {date_local(end)} |")

    lines.extend([
        "",
        "### 32.2 Antardaśā schedule through 2034",
        "",
        "| MD | AD | Start | End |",
        "|---|---|---:|---:|",
    ])
    for md, ad, start, end in ad_rows:
        if start.year > 2034:
            break
        lines.append(f"| {md} | {ad} | {date_local(start)} | {date_local(end)} |")

    lines.extend([
        "",
        "### 32.3 Detailed current/future MD–AD–PD–SD schedule through 2034",
        "",
        "This detailed table begins with Jupiter mahādaśā, the current MD, and runs through December 31, 2034.",
        "",
        "| MD | AD | PD | SD | Start | End |",
        "|---|---|---|---|---:|---:|",
    ])
    for md, ad, pd, sd, start, end in detailed_rows:
        lines.append(f"| {md} | {ad} | {pd} | {sd} | {date_local(start)} | {date_local(end)} |")

    lines.extend([
        "",
        "## 33. Consolidated Chronological Event Ledger",
        "",
        "| Date/time | Event | Observed outcome/details | Approximate daśā mapping |",
        "|---|---|---|---|",
    ])
    for date, event, outcome, dasha in EVENTS:
        lines.append(f"| {date} | {event} | {outcome} | {dasha} |")
    lines.extend(["", END, ""])
    return "\n".join(lines)


def main() -> None:
    profile = PROFILE.read_text(encoding="utf-8")
    generated = build_appendix()
    if BEGIN in profile and END in profile:
        prefix = profile.split(BEGIN, 1)[0].rstrip()
        suffix = profile.split(END, 1)[1].lstrip()
        updated = f"{prefix}\n\n{generated}"
        if suffix:
            updated += f"\n{suffix}"
    else:
        updated = profile.rstrip() + "\n\n" + generated
    PROFILE.write_text(updated, encoding="utf-8")


if __name__ == "__main__":
    main()
