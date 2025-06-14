-- Users table
CREATE TABLE Users (
    UserID INTEGER PRIMARY KEY AUTOINCREMENT,
    Username TEXT,
    Email TEXT,
    Password TEXT,
    LoggedIn BOOLEAN,
    Session INTEGER
);

-- Privilege table
CREATE TABLE Privileges (
    PrivilegeID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER,
    Name TEXT,
    Tier INTEGER,
    FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- LastLoggedIn table
CREATE TABLE LastLoggedIn (
    LoginID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER,
    DayName TEXT,
    DayNumber INTEGER,
    Hour INTEGER,
    Minute INTEGER,
    Second INTEGER,
    Month INTEGER,
    Week INTEGER,
    Year INTEGER,
    FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- Table (Student info per user)
CREATE TABLE UserTable (
    EntryID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER,
    ID TEXT,
    Name TEXT,
    Age TEXT,
    Class TEXT,
    Math TEXT,
    English TEXT,
    History TEXT,
    Religion TEXT,
    Swedish TEXT,
    FOREIGN KEY (UserID) REFERENCES Users(UserID)
);
