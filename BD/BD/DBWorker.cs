using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Configuration;

namespace BD
{
    public enum HeaderCellType{ Common, Primary, Foreign}

    public struct HeaderCell {
        public string value; public Type dataType;
        public HeaderCellType cellType;
        public string toTableName, toColumnName;

        public HeaderCell(string value, Type type, HeaderCellType cellType, string toTableName = "", string toColumnName="")
        {
            this.dataType = type;
            this.value = value;
            this.cellType = cellType;
            this.toTableName = toTableName;
            this.toColumnName = toColumnName;
        }
    }

    class DBWorker
    {
        static public DataTable GetTable(string commandText)
        {
            DataTable tempDataTable = new DataTable();
            string connectionString = ConfigurationManager.ConnectionStrings["DefaultConnection"].ConnectionString;
            string sqlExpression = commandText;
            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                SqlCommand command = new SqlCommand(sqlExpression, connection);
                SqlDataReader reader = command.ExecuteReader();
                object[] tempArray = new object[reader.FieldCount];

                for (int i = 0; i < reader.FieldCount; i++)
                    tempDataTable.Columns.Add(reader.GetName(i), reader.GetFieldType(i));

                while (reader.Read())
                {
                    for (int i = 0; i < reader.FieldCount; i++)
                        tempArray[i] = reader.GetValue(i);
                    tempDataTable.Rows.Add(tempArray);
                }

                reader.Close();
            }

            return tempDataTable;
        }

        public static string[] GetPrimaryKeys(string tableName)
        {
            string connectionString = ConfigurationManager.ConnectionStrings["DefaultConnection"].ConnectionString;
            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                using (SqlDataAdapter adapter = new SqlDataAdapter("select * from ["+tableName+"]", connection))
                using (DataTable table = new DataTable(tableName))
                {
                    return adapter
                        .FillSchema(table, SchemaType.Mapped)
                        .PrimaryKey
                        .Select(c => c.ColumnName)
                        .ToArray();
                }
            }
        }

        public static DataTable getForeignKeys(string tableName)
        {
          string commandText = "SELECT "+
                "ccu.column_name AS SourceColumn "+
                ",kcu.table_name AS TargetTable "+
                ",kcu.column_name AS TargetColumn "+
                "FROM(INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE ccu "+
                "INNER JOIN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS rc "+
                "ON ccu.CONSTRAINT_NAME = rc.CONSTRAINT_NAME "+
                "INNER JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE kcu "+
                "ON kcu.CONSTRAINT_NAME = rc.UNIQUE_CONSTRAINT_NAME) "+
                "Where ccu.table_name =\'"+tableName+"\'";

            return GetTable(commandText);
        }

        public static object DoScalarSQL(string commandText)
        {
            try
            {
                string connectionString = ConfigurationManager.ConnectionStrings["DefaultConnection"].ConnectionString;
                string sqlExpression = commandText;
                using (SqlConnection connection = new SqlConnection(connectionString))
                {
                    connection.Open();
                    SqlCommand command = new SqlCommand(sqlExpression, connection);
                    return command.ExecuteScalar();
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show(ex.Message);
                return null;
            }
            
        }
    }

    public class DataBaseInformation
    {
        public DataBaseInformation(string tableName)
        {
            this.TableName = tableName;
        }

        public string[] PrimaryKeys { get; set; }
        public string[] ForeignKeys { get; set; }
        public DataTable Table { get; set; }
        public string TableName { get; set; }
        public HeaderCell[] Headers { get; set; }

        public void Update(string sqlCommand)
        {
            PrimaryKeys = DBWorker.GetPrimaryKeys(TableName);
            var ForeignKeysTable = DBWorker.getForeignKeys(TableName);
            ForeignKeys = new string[ForeignKeysTable.Rows.Count];
            Table = DBWorker.GetTable(sqlCommand);
            Headers = new HeaderCell[Table.Columns.Count];

            for (int i = 0; i < ForeignKeys.Length; i++)
                ForeignKeys[i] = ForeignKeysTable.Rows[i].ItemArray[0].ToString();

            for (int i = 0; i < Headers.Length; i++)
            {
                string tempColName = Table.Columns[i].ColumnName;
                HeaderCellType hct;
                string col = "";
                string table = "";

                if (PrimaryKeys.Contains(tempColName)) hct = HeaderCellType.Primary;
                else
                {
                    int pos = Array.IndexOf(ForeignKeys, tempColName);
                    if(pos!=-1)
                    {
                        hct = HeaderCellType.Foreign;
                        table = ForeignKeysTable.Rows[pos].ItemArray[1].ToString();
                        col = ForeignKeysTable.Rows[pos].ItemArray[2].ToString();
                    } else
                        hct = HeaderCellType.Common;
                }    

                Headers[i] = new HeaderCell(Table.Columns[i].ColumnName, Table.Columns[i].DataType, hct, table, col);
            }

        }
    }
}
