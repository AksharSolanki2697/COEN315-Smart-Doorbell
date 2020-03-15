import sqlite3
from sqlite3 import Error

def create_connection(db_file):
    conn = None

    try: 
        conn = sqlite3.connect(db_file)
        return conn

       #print(sqlite3.version)
    except Error as e:
        print(e)

def create_table(conn, create_table_sql):
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)


def main():
   database_name =  "test.db"

   conn = create_connection(database_name)
   



   sql_create_projects_table = """ CREATE TABLE IF NOT EXISTS authenticate (
                                        username text PRIMARY KEY,
                                        password text 
                                    ); """

    
        # create tables
   if conn is not None:
        # create projects table
        create_table(conn, sql_create_projects_table)
   else:
        print("Error! cannot create the database connection.")


   conn.close()




if __name__ == '__main__':
    main()
