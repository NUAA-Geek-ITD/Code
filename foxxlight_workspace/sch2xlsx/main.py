from bs4 import BeautifulSoup
import xlwings as xl


def get_tag(row, col):
    return f"TD{col*11+row}_0"


def main():
    file = open(input("Html file path: "), mode="r", encoding="utf-8")
    soup = BeautifulSoup(file.read(), features="html.parser")
    file.close()

    wb = xl.Book()
    sheet = wb.sheets[0]

    sheet["A1"].value = soup.select("html body form#exportTableForm div#ExportA div")[0].string.replace("\n", "")

    sheet["B2"].value = "Mon"
    sheet["C2"].value = "Tue"
    sheet["D2"].value = "Wed"
    sheet["E2"].value = "Thu"
    sheet["F2"].value = "Fri"
    sheet["G2"].value = "Sat"
    sheet["H2"].value = "Sun"

    sheet["A3"].value = "1"
    sheet["A4"].value = "2"
    sheet["A5"].value = "3"
    sheet["A6"].value = "4"
    sheet["A7"].value = "5"
    sheet["A8"].value = "6"
    sheet["A9"].value = "7"
    sheet["A10"].value = "8"
    sheet["A11"].value = "9"
    sheet["A12"].value = "10"
    sheet["A13"].value = "11"

    ref = "BCDEFGH"

    for col in range(0, 7):
        merge_start = -1
        for row in range(0, 11):
            entry = soup.find(id=get_tag(row, col))
            if entry is None and merge_start == -1:
                merge_start = row
            elif entry is not None:
                title = entry.get("title")
                sheet[ref[col]+str(row+3)].value = title.replace(" ", "\n") if title is not None else None
                if merge_start != -1:
                    sheet[f"{ref[col]}{merge_start+2}:{ref[col]}{row+2}"].api.Merge()
                    merge_start = -1
        if merge_start != -1:
            sheet[f"{ref[col]}{merge_start + 2}:{ref[col]}{10+3}"].api.Merge()



if __name__ == '__main__':
    main()

