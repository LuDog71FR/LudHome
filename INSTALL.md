# Installation

## Database creation

Create a dedicated user for the application:

```bash
sudo -u postgres createuser ludhome -P --interactive
```

Create a database for the application and grant all privileges to the user:

```bash
sudo -u postgres psql
```

```sql  
create database ludhome;
grant all on schema public to ludhome;
```

## Database initialization

Create the tables:

```bash
sudo -u postgres psql -d ludhome -f src/sql/create_tables.sql
```

## Service creation

Create the user `ludhome` on the server:

```bash
sudo useradd ludhome
```

