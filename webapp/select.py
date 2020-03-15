import sqlite3
from sqlite3 import Error

def main(username):
    conn = None
    database =  "//var//www/Flask2//Flask2//sqlite3.db"

    conn = sqlite3.connect(database)

    with conn:
        cur = conn.cursor()
        cur.execute("SELECT password FROM projects WHERE username = ?",(username,))
        rows = cur.fetchall()
    conn.close()
    if rows:
        return rows[0][0]
    return None

if __name__ == "__main__":
    main(username)
