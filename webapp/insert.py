import sqlite3
from sqlite3 import Error

def main(username, password):
    conn = None
    database = "//var//www//Flask2//Flask2//sqlite3.db"
 
    # create a database connection
    conn = sqlite3.connect(database)
    with conn:
        # create a new project
        project = (username,password)
       # insert  = create_project(conn, project)
        sql = "INSERT INTO projects(username,password) VALUES(?,?);"
        cur = conn.cursor()
        cur.execute(sql, project)
        conn.commit()
    conn.close()
 

def login(username,password):
    conn = None
    database =  "//var//www/Flask2//Flask2//sqlite3.db"

    conn = sqlite3.connect(database)
    
    with conn:
        project = (username,password)
        sql = "SELECT * FROM projects;"
        cur = conn.cursor()
        cur.execute(sql)
        conn.commit()
    conn.close()

if __name__ == "__main__":
    main(username,password)

