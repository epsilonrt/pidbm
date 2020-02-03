# piduino-database developer notes

## Update soc pin with soc_pin_num=sys_pin_num

### View

```sql
SELECT pin_name.name,
       pin.id,
       soc_pin_num,
       sys_pin_num
  FROM pin_number
       INNER JOIN
       pin ON pin_number.pin_id = pin.id
       INNER JOIN
       soc_has_pin ON soc_has_pin.pin_id = pin.id
       INNER JOIN
       pin_has_name ON pin_has_name.pin_id = pin.id
       INNER JOIN
       pin_name ON pin_name.id = pin_has_name.pin_name_id
 WHERE soc_has_pin.soc_id = 3 AND 
       pin.pin_type_id = 0 AND 
       pin_has_name.pin_mode_id = 0
 ORDER BY soc_pin_num;
```

### Update

```sql
UPDATE
  pin_number
SET
  pin_number.soc_pin_num = pin_number.sys_pin_num
  FROM pin_number
       INNER JOIN
       pin ON pin_number.pin_id = pin.id
       INNER JOIN
       soc_has_pin ON soc_has_pin.pin_id = pin.id
       INNER JOIN
       pin_has_name ON pin_has_name.pin_id = pin.id
       INNER JOIN
       pin_name ON pin_name.id = pin_has_name.pin_name_id
 WHERE soc_has_pin.soc_id = 3 AND 
       pin.pin_type_id = 0 AND 
       pin_has_name.pin_mode_id = 0;
```

## search duplicate records

### pin names

```sql
SELECT pin_name.name,
       pin.id,
       count( * ) 
  FROM pin
       INNER JOIN
       pin_has_name ON pin_has_name.pin_id = pin.id
       INNER JOIN
       pin_name ON pin_name.id = pin_has_name.pin_name_id
 WHERE pin_has_name.pin_mode_id = 0
 GROUP BY pin_name.name
HAVING count( * ) > 1;
```

### soc_has_pin

```sql
SELECT pin_id,
       count( * ) 
  FROM soc_has_pin
 WHERE soc_id = 4
 GROUP BY pin_id
HAVING count( * ) > 1;
```

### pin_number

```sql
SELECT pin_id,
       count( * ) 
  FROM pin_number
 GROUP BY pin_id
HAVING count( * ) > 1;
```

### connector_has_pin

```sql
SELECT pin.id,
       pin_name.name,
       count( * ) 
  FROM connector_has_pin
       INNER JOIN
       pin ON pin.id = connector_has_pin.pin_id
       INNER JOIN
       pin_has_name ON pin.id = pin_has_name.pin_id
       INNER JOIN
       pin_name ON pin_name.id = pin_has_name.pin_name_id
 WHERE pin_type_id = 0 AND 
       pin_mode_id = 0 AND 
       connector_id = @connector_id
 GROUP BY pin.id
HAVING count( * ) > 1;
```

### gpio_has_pin

```sql
SELECT pin.id,
       pin_name.name,
       count( * ) 
  FROM gpio_has_pin
       INNER JOIN
       pin ON pin.id = gpio_has_pin.pin_id
       INNER JOIN
       pin_has_name ON pin.id = pin_has_name.pin_id
       INNER JOIN
       pin_name ON pin_name.id = pin_has_name.pin_name_id
 WHERE pin_type_id = 0 AND 
       pin_mode_id = 0 AND 
       gpio_id = @gpio_id
 GROUP BY pin.id
HAVING count( * ) > 1;
```


## import from sql

    sqlite3  < piduino-schema.sql
    sqlite3 piduino.db < piduino-data.sql
    
## import from csv

https://www.sqlitetutorial.net/sqlite-import-csv/

## export schema vers sql

    .output schema.sql
    .schema

## export data vers sql

    echo "BEGIN TRANSACTION;" >  piduino-data.sql; sqlite3 piduino.db .dump | grep '^INSERT INTO' | grep -v 'sqlite_sequence' >> piduino-data.sql; echo "COMMIT;" >>  piduino-data.sql

## export table vers sql de données

    .mode insert
    .output data.sql
    select * from board;

## export vers csv de données

    .mode csv
    .header on
    .output data.csv
    select * from board;

## export depuis MySQL Workbench

https://github.com/tatsushid/mysql-wb-exportsqlite

puis

    sed -i -e '/^CREATE\ INDEX/d' piduino-schema.sql
    sed -i -e 's/piduino\.sdb/piduino\.db/g' piduino-schema.sql

ExportSQLite is a plugin for
[MySQL Workbench](http://www.mysql.com/products/workbench/) to export a schema
catalog as SQLite's CREATE SQL queries. This is a Python port of
[original Lua script](https://gist.github.com/mrprompt/7252060) for using with
MySQL Workbench 6.2 or above.

### Installation

1. Open MySQL Workbench
2. Choose "Install Plugin/Module..." in "Scripting" menu
3. Open `export_sqlite_grt.py` file
4. Restart MySQL Workbench

### Usage

Once you open a database model in MySQL Workbench, you can see "Export SQLite
CREATE script" in "Tools > Catalog" menu. Just choose it.

# Views

## pin_names

```sql
SELECT pin_id,
       pin_mode_id,
       pin_name_id,
       pin_name.name AS pin_name
  FROM pin_has_name
       INNER JOIN
       pin_name ON pin_has_name.pin_name_id = pin_name.id
 ORDER BY pin_id,
          pin_mode_id;
```

## gpio_has_pin

```sql
SELECT gpio.id AS gpio_id,
       pin.id AS pin_id,
       logical_num AS ino_pin_num,
       pin_name.name AS name
  FROM gpio
       INNER JOIN
       gpio_has_connector ON gpio_has_connector.gpio_id = gpio.id
       INNER JOIN
       connector ON connector.id = gpio_has_connector.connector_id
       INNER JOIN
       connector_has_pin ON connector_has_pin.connector_id = connector.id
       INNER JOIN
       pin ON connector_has_pin.pin_id = pin.id
       INNER JOIN
       pin_number ON pin_number.pin_id = pin.id
       INNER JOIN
       pin_has_name ON pin_has_name.pin_id = pin.id
       INNER JOIN
       pin_name ON pin_has_name.pin_name_id = pin_name.id
 WHERE pin_type_id = 0 AND 
       pin_mode_id = 0
 ORDER BY gpio.id,
          logical_num   
```
          
## soc_has_pin_name

```sql
SELECT soc_id,
       soc.name AS soc_name,
       pin_id,
       pin_name.name AS pin_name
  FROM soc_has_pin
       INNER JOIN
       pin ON soc_has_pin.pin_id = pin.id
       INNER JOIN
       soc ON soc_has_pin.soc_id = soc.id
       INNER JOIN
       pin_has_name ON pin_has_name.pin_id = pin.id
       INNER JOIN
       pin_name ON pin_has_name.pin_name_id = pin_name.id
 WHERE pin_mode_id = 0
 ORDER BY soc_id,
          pin_name.name;
```
          
