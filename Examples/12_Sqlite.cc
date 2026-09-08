/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : 12_Sqlite.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Generic SQLite browser powered by SqliteAdapter.
 *  @date       : 2026/09/08
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include "SqliteAdapter.h"

namespace {

std::string Trim(const std::string& text)
{
    const std::string whitespace = " \t\r\n";
    const size_t begin = text.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return "";
    }
    const size_t end = text.find_last_not_of(whitespace);
    return text.substr(begin, end - begin + 1);
}

std::string EscapeSqlText(const std::string& text)
{
    std::string out;
    out.reserve(text.size());
    for (char ch : text) {
        if (ch == '\'') {
            out.push_back('\'');
        }
        out.push_back(ch);
    }
    return out;
}

std::string QuoteIdent(const std::string& value)
{
    std::string out;
    out.reserve(value.size() + 2);
    out.push_back('"');
    for (char ch : value) {
        if (ch == '"') {
            out.push_back('"');
        }
        out.push_back(ch);
    }
    out.push_back('"');
    return out;
}

std::string ReadLine(const std::string& prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

void ClearScreen()
{
    std::cout << "\033[2J\033[H";
}

void WaitForEnter(const std::string& message = "Press Enter to continue...")
{
    std::cout << message << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void PrintSectionHeader(const std::string& title)
{
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

bool QueryRows(SqliteAdapter* db, const std::string& sql, std::vector<std::vector<std::string>>& rows)
{
    if (db == nullptr) {
        return false;
    }
    return db->Query(sql, rows);
}

std::vector<std::string> ListTables(SqliteAdapter* db)
{
    std::vector<std::string> tables;
    std::vector<std::vector<std::string>> rows;
    if (!QueryRows(db, "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name;", rows)) {
        return tables;
    }
    for (const auto& row : rows) {
        if (!row.empty()) {
            tables.push_back(row[0]);
        }
    }
    return tables;
}

bool GetColumns(SqliteAdapter* db, const std::string& tableName, std::vector<std::string>& columns)
{
    columns.clear();
    std::vector<std::vector<std::string>> rows;
    const std::string sql = "PRAGMA table_info(" + QuoteIdent(tableName) + ");";
    if (!QueryRows(db, sql, rows)) {
        return false;
    }
    for (const auto& row : rows) {
        if (row.size() > 1 && !row[1].empty()) {
            columns.push_back(row[1]);
        }
    }
    return true;
}

void PrintRows(const std::vector<std::vector<std::string>>& rows, const std::vector<std::string>& headers)
{
    if (rows.empty()) {
        std::cout << "  [no rows]" << std::endl;
        return;
    }

    std::vector<size_t> widths(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); ++i) {
        widths[i] = headers[i].size();
    }

    for (const auto& row : rows) {
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i < row.size()) {
                widths[i] = std::max(widths[i], row[i].size());
            } else {
                widths[i] = std::max(widths[i], static_cast<size_t>(4));
            }
        }
    }

    auto printRow = [&](const std::vector<std::string>& row) {
        for (size_t i = 0; i < headers.size(); ++i) {
            std::string value = (i < row.size()) ? row[i] : "";
            std::cout << "  " << value;
            if (i + 1 < headers.size()) {
                std::cout << std::string(widths[i] - value.size() + 2, ' ');
            }
        }
        std::cout << std::endl;
    };

    printRow(headers);
    for (size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::string(widths[i] + 2, '-');
        if (i + 1 < headers.size()) {
            std::cout << "  ";
        }
    }
    std::cout << std::endl;

    for (const auto& row : rows) {
        printRow(row);
    }
}

bool ShowTableRows(SqliteAdapter* db, const std::string& tableName, int limit)
{
    std::vector<std::string> columns;
    if (!GetColumns(db, tableName, columns) || columns.empty()) {
        std::cout << "Error: table '" << tableName << "' has no columns." << std::endl;
        return false;
    }

    std::vector<std::vector<std::string>> rows;
    std::string sql = "SELECT * FROM " + QuoteIdent(tableName) + " LIMIT " + std::to_string(limit) + ";";
    if (!QueryRows(db, sql, rows)) {
        std::cout << "Error: unable to read table '" << tableName << "'." << std::endl;
        return false;
    }

    std::cout << "Table: " << tableName << " (limit " << limit << ")" << std::endl;
    PrintRows(rows, columns);
    return true;
}

bool ShowTableSchema(SqliteAdapter* db, const std::string& tableName)
{
    std::vector<std::vector<std::string>> rows;
    const std::string sql = "PRAGMA table_info(" + QuoteIdent(tableName) + ");";
    if (!QueryRows(db, sql, rows)) {
        std::cout << "Error: unable to read schema for '" << tableName << "'." << std::endl;
        return false;
    }
    if (rows.empty()) {
        std::cout << "Table not found: " << tableName << std::endl;
        return false;
    }

    std::vector<std::string> headers = {"cid", "name", "type", "notnull", "dflt_value", "pk"};
    std::cout << "Schema: " << tableName << std::endl;
    PrintRows(rows, headers);
    return true;
}

bool InsertRow(SqliteAdapter* db, const std::string& tableName)
{
    std::vector<std::string> columns;
    if (!GetColumns(db, tableName, columns)) {
        std::cout << "Error: cannot inspect table '" << tableName << "'." << std::endl;
        return false;
    }

    std::vector<std::string> values;
    values.reserve(columns.size());
    std::cout << "Enter values for " << tableName << ":" << std::endl;
    for (const auto& column : columns) {
        std::string value = ReadLine("  " + column + " => ");
        if (value.empty() && columns.size() == 1) {
            std::cout << "Error: value cannot be empty." << std::endl;
            return false;
        }
        values.push_back(value);
    }

    std::stringstream cols;
    std::stringstream vals;
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) {
            cols << ", ";
            vals << ", ";
        }
        cols << QuoteIdent(columns[i]);
        vals << "'" << EscapeSqlText(values[i]) << "'";
    }

    std::string sql = "INSERT INTO " + QuoteIdent(tableName) + " (" + cols.str() + ") VALUES (" + vals.str() + ");";
    if (!db->Execute(sql)) {
        std::cout << "Error: insert failed." << std::endl;
        return false;
    }
    std::cout << "Inserted into " << tableName << "." << std::endl;
    return true;
}

bool UpdateRow(SqliteAdapter* db, const std::string& tableName)
{
    std::vector<std::string> columns;
    if (!GetColumns(db, tableName, columns) || columns.empty()) {
        std::cout << "Error: no editable columns in " << tableName << "." << std::endl;
        return false;
    }

    std::cout << "Current rows:" << std::endl;
    ShowTableRows(db, tableName, 20);

    std::string pkColumn = columns.front();
    std::string pkValue = ReadLine("  primary-key value => ");
    if (pkValue.empty()) {
        std::cout << "Error: primary-key value required." << std::endl;
        return false;
    }

    std::string colName = ReadLine("  update column => ");
    if (colName.empty()) {
        std::cout << "Error: column name required." << std::endl;
        return false;
    }
    if (std::find(columns.begin(), columns.end(), colName) == columns.end()) {
        std::cout << "Error: unknown column '" << colName << "'." << std::endl;
        return false;
    }

    std::string newValue = ReadLine("  new value => ");
    std::string sql = "UPDATE " + QuoteIdent(tableName) + " SET " + QuoteIdent(colName) + "='" + EscapeSqlText(newValue) + "' WHERE " + QuoteIdent(pkColumn) + "='" + EscapeSqlText(pkValue) + "';";
    if (!db->Execute(sql)) {
        std::cout << "Error: update failed." << std::endl;
        return false;
    }
    std::cout << "Updated row in " << tableName << "." << std::endl;
    return true;
}

bool DeleteRow(SqliteAdapter* db, const std::string& tableName)
{
    std::vector<std::string> columns;
    if (!GetColumns(db, tableName, columns) || columns.empty()) {
        std::cout << "Error: no columns in " << tableName << "." << std::endl;
        return false;
    }

    std::cout << "Current rows:" << std::endl;
    ShowTableRows(db, tableName, 20);

    std::string pkColumn = columns.front();
    std::string pkValue = ReadLine("  delete by primary key => ");
    if (pkValue.empty()) {
        std::cout << "Error: primary-key value required." << std::endl;
        return false;
    }

    std::string sql = "DELETE FROM " + QuoteIdent(tableName) + " WHERE " + QuoteIdent(pkColumn) + "='" + EscapeSqlText(pkValue) + "';";
    if (!db->Execute(sql)) {
        std::cout << "Error: delete failed." << std::endl;
        return false;
    }
    std::cout << "Deleted row from " << tableName << "." << std::endl;
    return true;
}

bool ExecuteCustomSql(SqliteAdapter* db)
{
    std::string sql = ReadLine("SQL> ");
    if (Trim(sql).empty()) {
        return true;
    }

    std::string upper = Trim(sql);
    std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });

    std::vector<std::vector<std::string>> rows;
    if (upper.find("SELECT") == 0 || upper.find("PRAGMA") == 0) {
        if (!QueryRows(db, sql, rows)) {
            std::cout << "Error: query failed." << std::endl;
            return false;
        }
        std::cout << "Query result:" << std::endl;
        if (rows.empty()) {
            std::cout << "  [no rows]" << std::endl;
            return true;
        }
        std::vector<std::string> headers;
        for (size_t i = 0; i < rows[0].size(); ++i) {
            headers.push_back(std::string("col") + std::to_string(i + 1));
        }
        PrintRows(rows, headers);
        return true;
    }

    if (!db->Execute(sql)) {
        std::cout << "Error: SQL execution failed." << std::endl;
        return false;
    }
    std::cout << "Executed successfully." << std::endl;
    return true;
}

void PrintMenu()
{
    PrintSectionHeader("SQLite Explorer");
    std::cout << "  1) List tables" << std::endl;
    std::cout << "  2) Show schema" << std::endl;
    std::cout << "  3) Browse table" << std::endl;
    std::cout << "  4) Insert row" << std::endl;
    std::cout << "  5) Update row" << std::endl;
    std::cout << "  6) Delete row" << std::endl;
    std::cout << "  7) Custom SQL" << std::endl;
    std::cout << "  0) Exit" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}

std::string ChooseTable(SqliteAdapter* db)
{
    const std::vector<std::string> tables = ListTables(db);
    if (tables.empty()) {
        std::cout << "No tables found." << std::endl;
        return "";
    }

    std::cout << "Tables:" << std::endl;
    for (size_t i = 0; i < tables.size(); ++i) {
        std::cout << "  [" << i + 1 << "] " << tables[i] << std::endl;
    }

    std::string input = ReadLine("Select table => ");
    if (Trim(input).empty()) {
        return "";
    }

    const std::string trimmed = Trim(input);
    if (std::all_of(trimmed.begin(), trimmed.end(), [](unsigned char ch) { return std::isdigit(ch); })) {
        const int index = std::stoi(trimmed) - 1;
        if (index >= 0 && static_cast<size_t>(index) < tables.size()) {
            return tables[index];
        }
        std::cout << "Error: invalid table index." << std::endl;
        return "";
    }

    if (std::find(tables.begin(), tables.end(), trimmed) != tables.end()) {
        return trimmed;
    }

    std::cout << "Error: unknown table name." << std::endl;
    return "";
}

}  // namespace

int main(int argc, const char* argv[])
{
    std::string dbPath;
    if (argc > 1) {
        dbPath = argv[1];
    } else {
        dbPath = ReadLine("Database path => ");
    }

    dbPath = Trim(dbPath);
    if (dbPath.empty()) {
        std::cout << "Usage: " << argv[0] << " <database-file>" << std::endl;
        return 1;
    }

    SqliteAdapter* db = SqliteAdapter::GetInstance(dbPath);
    if (db == nullptr) {
        std::cout << "Error: cannot open database '" << dbPath << "'." << std::endl;
        return 1;
    }

    std::cout << "Connected: " << dbPath << std::endl;
    std::cout << "Tip: use the menu to browse or edit data without writing SQL." << std::endl;

    while (true) {
        ClearScreen();
        PrintMenu();
        std::string choice = ReadLine("Choice => ");
        int cmd = -1;
        try {
            if (!choice.empty()) {
                cmd = std::stoi(choice);
            }
        } catch (...) {
            std::cout << "Error: invalid choice. Please select 0-7." << std::endl;
            WaitForEnter();
            continue;
        }

        if (cmd == 0) {
            ClearScreen();
            std::cout << "Bye." << std::endl;
            break;
        }

        ClearScreen();
        switch (cmd) {
            case 1: {
                PrintSectionHeader("Tables");
                const std::vector<std::string> tables = ListTables(db);
                if (tables.empty()) {
                    std::cout << "No tables found." << std::endl;
                    break;
                }
                for (size_t i = 0; i < tables.size(); ++i) {
                    std::cout << "  [" << i + 1 << "] " << tables[i] << std::endl;
                }
                break;
            }
            case 2: {
                PrintSectionHeader("Schema");
                const std::string tableName = ChooseTable(db);
                if (!tableName.empty()) {
                    ShowTableSchema(db, tableName);
                }
                break;
            }
            case 3: {
                PrintSectionHeader("Browse Table");
                const std::string tableName = ChooseTable(db);
                if (!tableName.empty()) {
                    ShowTableRows(db, tableName, 20);
                }
                break;
            }
            case 4: {
                PrintSectionHeader("Insert Row");
                const std::string tableName = ChooseTable(db);
                if (!tableName.empty()) {
                    InsertRow(db, tableName);
                }
                break;
            }
            case 5: {
                PrintSectionHeader("Update Row");
                const std::string tableName = ChooseTable(db);
                if (!tableName.empty()) {
                    UpdateRow(db, tableName);
                }
                break;
            }
            case 6: {
                PrintSectionHeader("Delete Row");
                const std::string tableName = ChooseTable(db);
                if (!tableName.empty()) {
                    DeleteRow(db, tableName);
                }
                break;
            }
            case 7: {
                PrintSectionHeader("Custom SQL");
                ExecuteCustomSql(db);
                break;
            }
            default:
                std::cout << "Error: invalid choice. Please select 0-7." << std::endl;
                break;
        }

        WaitForEnter();
    }

    return 0;
}

