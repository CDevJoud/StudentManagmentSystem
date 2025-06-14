-- User 1: admin
INSERT INTO Users (Username, Email, Password, LoggedIn, Session) VALUES 
('admin', 'admin@lol.com', '12345', 0, 2025);

INSERT INTO Privileges (UserID, Name, Tier) VALUES (1, 'admin', 5);

INSERT INTO LastLoggedIn (UserID, DayName, DayNumber, Hour, Minute, Second, Month, Week, Year) VALUES 
(1, 'Monday', 24, 4, 45, 30, 1, 45, 2025);

INSERT INTO UserTable (UserID, ID, Name, Age, Class, Math, English, History, Religion, Swedish) VALUES 
(1, '1', 'Joud', '19', 'Te22I', 'a', 'a', 'a', 'b', 'A');

-- User 2: Hazzy
INSERT INTO Users (Username, Email, Password, LoggedIn, Session) VALUES 
('Hazzy', 'admin@lol.com', 'IHateBea', 0, 2026);

INSERT INTO Privileges (UserID, Name, Tier) VALUES (2, 'admin', 5);

INSERT INTO LastLoggedIn (UserID, DayName, DayNumber, Hour, Minute, Second, Month, Week, Year) VALUES 
(2, 'Monday', 24, 4, 45, 30, 1, 45, 2025);

-- User 3: Joud
INSERT INTO Users (Username, Email, Password, LoggedIn, Session) VALUES 
('Joud', 'admin@lol.com', '1234', 1, 2005);

INSERT INTO Privileges (UserID, Name, Tier) VALUES (3, 'admin', 5);

INSERT INTO LastLoggedIn (UserID, DayName, DayNumber, Hour, Minute, Second, Month, Week, Year) VALUES 
(3, 'Monday', 24, 4, 45, 30, 1, 45, 2025);

-- Insert multiple rows into UserTable for Joud (UserID = 3)
INSERT INTO UserTable (UserID, ID, Name, Age, Class, Math, English, History, Religion, Swedish) VALUES
(3, '0', 'Joud Kandeel', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '1', 'Fernando', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '2', 'Precious', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '3', 'Hani', '19', 'TE22I', 'A', 'A', 'B', 'A', 'A'),
(3, '4', 'Abdullah', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '5', 'Amer', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '6', 'Helin', '12', 'TE22I', 'F-', 'A+', 'F-', 'F-', 'F-'),
(3, '7', 'Rasha', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '8', 'Hazzy', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '9', 'Leen', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A'),
(3, '10', 'Hala', '19', 'TE22I', 'A', 'A', 'A', 'A', 'A');