// Trans30.h : main header file for the TRANS30 application
//

#if !defined(_TRANS30_H)
#define _TRANS30_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "DataVal.h"

#define SECS_TO_FT_MULT 10000000

/////////////////////////////////////////////////////////////////////
// Глобальные сообщения
//

#define MDI_UPDATE_BAR      (WM_USER + 1)
#define FORM_SET_STATE      (WM_USER + 2)
#define FORM_GET_STATE      (WM_USER + 3)
#define FORM_UPDATE_DATA    (WM_USER + 4)
#define FORM_UPDATE_MENU    (WM_USER + 5)
#define FORM_QUERY_SAVE     (WM_USER + 6)
#define TRAY_NOTIFYICON		(WM_APP + 100)

// Состояния формы
enum FORM_STATE {
	FORM_EDIT,			// Редактирование
	FORM_NEW			// Ввод
};

// Схемы трансферта
enum TRN_SCHEMA {
	TRN_COLUMNS,		// Таблица
	TRN_COPY,			// Копия
	TRN_COMPLEX			// Комплекс
};

// Схемы трансферта
enum TRN_CONVERT {
	TRN_UNCHANGED,		// Неизменияемый
	TRN_OEMTOANSI,		// OEM в ANSI
	TRN_ANSITOOEM		// ANSI в OEM
};

// Вид обработки таблицы
enum DST_PROCESS {
    DST_CREATE,			// Создание
    DST_ADD,			// Добавление
    DST_REPLACE,		// Замещение
	DST_FULL			// Полное сравнение
};

// Типы источников данных
enum SRC_TYPE {
	DATA_LOCAL,			// Значение
	DATA_AUTO,			// Автоинкремент
	DATA_PARAM,			// Параметр
	DATA_CONST,			// Константа
	DATA_TABSRC,		// Основной источник
	DATA_SUBSRC,		// Подчиненный источник
	DATA_TABDST,		// Основной приемник
	DATA_SUBDST,		// Подчиненный приемник
	DATA_REF,			// Переопределение
	DATA_FUNC,			// Функция
	DATA_PROC			// Процедура
};

// Флаги полей
enum FLD_FLAG {
	FLD_DATA,			// Просто данные
	FLD_KEY,			// Уникальный ключ
	FLD_NOCHANGE,		// Неизменяемое поле
	FLD_EVCHANGE,		// Команда при изменении
	FLD_FRUNBACK,		// Флаг выполнения/отката
	FLD_TRIGGER			// Триггер при изменении
};

// Типы событий
enum EV_TYPE {
	EV_UNKNOWN,			// Неопределенное
	EV_ON_START,		// На старте
	EV_ON_END,			// При окончании
	EV_CALL_EXEC,		// При проведении
	EV_CALL_BACK,		// При откате
	EV_ON_INSERT,		// При вставке строки
	EV_ON_UPDATE,		// При изменении строки
	EV_ON_DELETE,		// При удалении строки
	EV_RE_INSERT,		// При отмене вставки
	EV_RE_UPDATE,		// При отмене изменения
	EV_RE_DELETE,		// При отмене удаления
	EV_ON_CHANGE,		// При изменении поля
	EV_RE_CHANGE		// При отмене изменения поля
};

// Статус строки
enum ROW_STATUS {
	ROW_UNKNOWN,		// Неопределенный
	ROW_DELETED,		// Строка удалена
	ROW_MODIFIED,		// Строка изменена
	ROW_INSERTED,		// Строка вставлена
	ROW_CMDONDEL,		// Команда ON_DELETE
	ROW_CMDONUPD,		// Команда ON_UPDATE
	ROW_CMDONINS		// Команда ON_INSERT
};

// Типы встроенных функций
enum FNC_TYPE {
	FNC_SQLGETVAL,		// Запрос значения поля
	FNC_SQLMAXVAL,		// Запрос максимального значения поля
	FNC_SQLMINVAL,		// Запрос миниимального значения поля
	FNC_SQLAVGVAL,		// Запрос среднего значения поля
	FNC_SQLSUMVAL,		// Запрос суммы значений поля
	FNC_SQLVARVAL,		// Запрос вариации значений поля
	FNC_SQLCOUNT		// Запрос количества записей 
};

// Типы ошибок
enum ER_TYPE {
    ER_UNKNOWN,			// Неопределенная
    ER_NOTVALID,		// Нет готовности
	ER_NO_PPL,			// Не указан файл профиля
	ER_NO_EXPR,			// Пустое выражение
	ER_NO_TABLE,		// Не указано имя таблицы
	ER_NO_KEYS,			// Не установлено ключевых полей
	ER_NO_FIELDS,		// Результирующий набор не содержит полей
	ER_NO_MODFLD,		// Не обнаружено изменяемых полей
	ER_NEED_CODE,		// Введите код!
	ER_NEED_NAME,		// Введите наименование!
	ER_NEED_FILE,		// Введите имя файла!
	ER_NEED_VALUE,		// Введите значение!
	ER_NEED_FIELD,		// Введите имя поля!
	ER_NEED_COMMAND,	// Введите команду!
	ER_NEED_SQLTAB,		// Введите SQL запрос таблицы источника!
	ER_NEED_SQLSUB,		// Введите SQL запрос субтаблицы источника!
	ER_IGNORE_SQLSUB,	// SQL запрос субтаблицы не может использоваться!
	ER_DOUBLE_FIELD,	// Имя поля уже успользуется
	ER_DOUBLE_COND,		// Условие уже успользуется
	ER_DOUBLE_CODE,		// Код уже успользуется
	ER_FIND_FILE,		// Файл не найден
	ER_READ_FILE,		// Ошибка чтения файла
	ER_OPEN_FILE,		// Невозможно открыть файл
	ER_WRITE_FILE,		// Ошибка записи в файл
	ER_CONN_SRC,		// Ошибка соединения с БД источника
	ER_CONN_DST,		// Ошибка соединения с БД приемника
    ER_INSTAB,			// Ошибка SQL Insert таблицы
    ER_UPDTAB,			// Ошибка SQL Update таблицы
	ER_DELTAB,			// Ошибка SQL Delete таблицы
    ER_INSSUB,			// Ошибка SQL Insert субтаблицы
    ER_UPDSUB,			// Ошибка SQL Update субтаблицы
	ER_DELSUB,			// Ошибка SQL Delete субтаблицы
	ER_SNTX_PARAM,		// Ошибка синтаксиса параметра
	ER_SNTX_PARAMS,		// Ошибка синтаксиса параметров
	ER_SNTX_MAPTAB,		// Ошибка синтаксиса карты полей таблицы
	ER_SNTX_MAPSUB,		// Ошибка синтаксиса карты полей субтаблицы
	ER_SNTX_CMDTAB,		// Ошибка синтаксиса SQL-команд таблицы
	ER_SNTX_CMDSUB,		// Ошибка синтаксиса SQL-команд субтаблицы
	ER_MAIN_TABLE,		// Ошибка определения основной таблицы
	ER_CRT_SQL,			// Ошибка создания дескриптора SQL
	ER_CRT_TABLE,		// Ошибка создания таблицы
	ER_CRT_INDEX,		// Ошибка создания уникального индекса
	ER_CRT_TAB,			// Создание таблицы приемника
	ER_CRT_SUB,			// Создание субтаблицы приемника
	ER_PREP_SRCTAB,		// Подготовка таблицы источника
	ER_PREP_SRCSUB,		// Подготовка субтаблицы источника
	ER_PREP_DSTTAB,		// Подготовка таблицы приемника
	ER_PREP_DSTSUB,		// Подготовка субтаблицы приемника
	ER_PREP_MSG,		// Подготовка строки сообщения
	ER_PREP_INSTAB,		// Подготовка SQL Insert таблицы
	ER_PREP_UPDTAB,		// Подготовка SQL Update таблицы
	ER_PREP_DELTAB,		// Подготовка SQL Delete таблицы
	ER_PREP_INSSUB,		// Подготовка SQL Insert субтаблицы
	ER_PREP_UPDSUB,		// Подготовка SQL Update субтаблицы
	ER_PREP_DELSUB,		// Подготовка SQL Delete субтаблицы
	ER_PREP_CMDTAB,		// Подготовка SQL-команд таблицы
	ER_PREP_CMDSUB,		// Подготовка SQL-команд субтаблицы
	ER_PREP_PARSRCTAB,	// Подготовка параметров таблицы источника
	ER_PREP_SELSRCTAB,	// Подготовка SQL Select таблицы источника
	ER_PREP_FNDSRCTAB,	// Подготовка SQL запроса к таблице источника
	ER_PREP_PARSRCSUB,	// Подготовка параметров субтаблицы источника
	ER_PREP_SELSRCSUB,	// Подготовка SQL Select субтаблицы источника
	ER_PREP_FNDSRCSUB,	// Подготовка SQL запроса к субтаблице источника
	ER_PREP_PARDSTTAB,	// Подготовка параметров таблицы приемника
	ER_PREP_SELDSTTAB,	// Подготовка SQL Select таблицы приемника
	ER_PREP_FNDDSTTAB,	// Подготовка SQL запроса к таблице приемника
	ER_PREP_PARDSTSUB,	// Подготовка параметров субтаблицы приемника
	ER_PREP_SELDSTSUB,	// Подготовка SQL Select субтаблицы приемника
	ER_PREP_FNDDSTSUB,	// Подготовка SQL запроса к субтаблице приемника
	ER_COUNT_SRCTAB,	// Подсчет строк в таблице источника
	ER_COUNT_DSTTAB,	// Подсчет строк в таблице приемника
	ER_READ_SRCTAB,		// Чтение данных из таблицы источника
	ER_READ_DSTTAB,		// Чтение данных из таблицы приемника
	ER_READ_SRCSUB,		// Чтение данных из субтаблицы источника
	ER_READ_DSTSUB,		// Чтение данных из субтаблицы источника
	ER_FIND_SRCTAB,		// Поиск строки в таблице источника
	ER_FIND_DSTTAB,		// Поиск строки в таблице приемника
	ER_FIND_SRCSUB,		// Поиск строки в субтаблице источника
	ER_FIND_DSTSUB,		// Поиск строки в субтаблице приемника
	ER_FIND_PARAM,		// Не найден параметр
	ER_FIND_CONST,		// Не найдена константа
	ER_FIND_FLDKEY,		// Не найдено ключевое поле
	ER_FIND_FLDMAP,		// Не найдено поле в карте полей
	ER_FIND_FLDSRCTAB,	// Не найдено поле таблицы источника
	ER_FIND_FLDSRCSUB,	// Не найдено поле субтаблицы источника
	ER_FIND_FLDDSTREF,	// Не найдено поле со ссылкой на источник
	ER_FIND_FLDDSTTAB,	// Не найдено поле таблицы приемника
	ER_FIND_FLDDSTSUB,	// Не найдено поле субтаблицы приемника
	ER_FIND_FUNC,		// Не найдена функция
	ER_FIND_PROC,		// Не найдена процедура
	ER_BAD_TYPE,		// Неизвестный тип
	ER_BAD_DATA,		// Неверное значение
	ER_BAD_CHAR,		// Недопустимый символ
	ER_BAD_VAL,			// Недопустимое значение
	ER_BAD_REF,			// Недопустимая ссылка
	ER_BAD_FLDCMD,		// Недопустимое поле для команды OnChange
	ER_COMP_DSTTAB,		// Сравнение данных в таблице приемника
	ER_COMP_DSTSUB,		// Сравнение данных в субтаблице приемника
	ER_CONV_PARTYPE,	// Ошибка конвертации типа параметра
	ER_CONV_FLDTYPE,	// Ошибка конвертации типа поля
	ER_CONV_PARAM,		// Ошибка конвертации значения параметра
	ER_CONV_CONST,		// Ошибка конвертации значения константы
	ER_CONV_FIELD,		// Ошибка конвертации значения поля
	ER_CONV_FUNC,		// Ошибка конвертации значения функции
	ER_CONV_PROC,		// Ошибка конвертации значения процедуры
	ER_DEFINE,			// Ошибка в описании
	ER_DEF_INDEX,		// Ошибка определения уникального индекса
	ER_DEF_REFTYPE,		// Невозможно определить тип по ссылке
	ER_DEF_FIELDS,		// Не определен массив полей
	ER_DEF_TABLE,		// Таблица не существует
	ER_DEF_KEYS,		// Определение ключевых полей
	ER_DEF_REFS,		// Установка ссылок
	ER_MOD_FLDKEY,		// Невозможно изменить ключевое поле
	ER_REPL_FLDKEY,		// Невозможно заменить ключевое поле
	ER_REPL_FLDDEF,		// Невозможно заменить уже определенное поле
	ER_PARSE_PARAMS,	// Ошибка разбора параметров
	ER_CMD_PARAMS,		// Ошибка в параметрах команды
	ER_DESC_FIELDS,		// Ошибка получения описания полей
	ER_DESC_PARAMS,		// Ошибка получения описания параметров
	ER_DESC_CATTAB,		// Ошибка получения списка таблиц
	ER_UNS_PARAMS,		// Недостаточно параметров
	ER_BIND_PARAM,		// Ошибка связывания параметра
	ER_BIND_FIELD,		// Ошибка связывания поля
	ER_BIND_PARAMS,		// Ошибка связывания параметров
	ER_BIND_FIELDS,		// Ошибка связывания полей данных
	ER_SET_PARAMS,		// Ошибка установки параметров
	ER_BACK_DETAIL,		// Ошибка восстановления спецификации
	ER_BACK_CMD,		// Ошибка выполнения обратной команды
	ER_BACK_OP			// Ошибка выполнения обратной операции
};

// Системные параметры
typedef struct _TRN_PARM {
	// Соединение с БД
	CString	sDSN;		// Источник ODBC
	CString	sUID;		// Идентификатор пользователя
	CString	sPWD;		// Пароль пользователя
	// Приложение
	CString	sNameApp;	// Наименование приложения
	CString	sDirApp;	// Директорий запуска
	// Общие
	CString sDirPpl;	// Директорий файлов трансферта
	CString	sDirLog;	// Каталог файла протокола
	CString	sDirMsg;	// Каталог файлов сообщений
	CString	sFileLog;	// Файл протокола
	CString	sFileMsg;	// Файл сообщений
	CString	sPplImp;	// Файл импорта по умолчанию
	CString sPplExp;	// Файл экспорта по умолчанию
	UINT	iUseImp;	// Импорт по умолчанию
	UINT	iUseExp;	// Экспорт по умолчанию
	// Интерфейс
	CString	sLangID;	// Идентификатор языка
	UINT	iSplashW;	// Показ окна "О программе" при запуске
	UINT	iMinimize;	// Признак минимизации при запуске
	UINT	iSysTray;	// Признак минимизации в SysTray
	UINT	iSavePos;	// Признак сохранения позиции главного окна
	UINT	iSaveMDI;	// Признак сохранения позиции MDI окон
	UINT	iMaxiMDI;	// Признак максимизации MDI окон
	// Обработка
	UINT	iDefType;	// Типы данных по умолчанию
	UINT	iAutoExc;	// Автозапуск обмена данными
	UINT	iDelFile;	// Удаление протокола при запуске
	UINT	iWriteOpr;	// Запись протокола операций в файл
	UINT	iWriteMsg;	// Запись сообщений в файл при импорте
	UINT	iShowErr;	// Выдавать сообщения об ошибках на экран
	UINT	iTraceOpr;	// Трассировать SQL-операторы
	UINT	iTraceCmd;	// Трассировать SQL-команды
	UINT	iTraceSel;	// Трассировать SQL-Select
	// Обмен данными
	UINT	iDepart;	// Подразделение
	UINT	iExcTime;	// Интервал обмена
	UINT	iIntTime;	// Интервал опроса
	CString	sFileImp;	// Файл импорта
	CString	sFileExp;	// Файл экспорта
	CString	sFileEmp;	// Файл пустой
	CString	sDirPut;	// Каталог передачи
	CString	sDirGet;	// Каталог приема
	CString	sNameArc;	// Имя архива
} TRN_PARM;

// Описание профиля
typedef struct _PPL {
	int		iNum;		// Номер
	int		iKind;		// Вид профиля
	CString	sFile;		// Файл описания
	CString	sName;		// Наименование
} PPL;

// Описание блока Трансферта
typedef struct _TRN_INFO {
	BOOL	bValid;		// Признак проверки
	BOOL	bSelect;	// Признак выбора
	UINT	iSchema;	// Схема трансферта
	UINT	iConvert;	// Конвертация текста
	CString	sName;		// Наименование
	CString	sMsg;		// Шаблон строки сообщения
	UINT	iTabPrc;	// Вид обработки приемника
	CString	sTabSQL;	// SQL Select таблицы источника
	CString	sTabOwn;	// Схема (владелец) приемника
	CString	sTabDst;	// Таблица приемника
	CString	sTabCond;	// Условие запроса применика
	CString	sTabMap;	// Карта полей приемника
	CString	sTabCmd;	// Команды приемника
	UINT	iSubPrc;	// Вид обработки субтаблицы
	CString	sSubSQL;	// SQL Select субтаблицы
	CString	sSubOwn;	// Схема (владелец) субтаблицы
	CString	sSubDst;	// Таблица субтаблицы
	CString	sSubCond;	// Условие запроса субтаблицы
	CString	sSubMap;	// Карта полей субтаблицы
	CString	sSubCmd;	// Команды субтаблицы
} TRN_INFO;

// Определение массива трансфертов
typedef CArray<TRN_INFO,TRN_INFO&> CTrnArray;

// Описание поля
typedef struct _FLD_INFO {
	UINT		iSQLType;	// SQL-Тип
	int			iSize;		// Размер
	int			iPrec;		// Точность
	int			fNull;		// Nullable
	int			iFlag;		// Флаг
	int			iSrc;		// Источник
	int			iRef;		// Ссылка
	int			iDst;		// Приемник
	int			iIdx;		// Индекс
	BOOL		bMod;		// Признак изменения
	CString		sID;		// Идентификатор
	CString		sName;		// Наименование
	CString		sType;		// Название типа
	CString		sValue;		// Значение
	CDataValue	vData;		// Данные
} FLD_INFO;

// Определение массива полей
typedef CArray<FLD_INFO,FLD_INFO&> CFldArray;

// Определение 2-мерного массива полей
typedef CTypedPtrArray<CPtrArray, CFldArray*> CArFldArray;

// Описание строки данных
typedef struct _ROW_INFO {
	long		nRow;		// Номер строки
	ROW_STATUS	nStatus;	// Статус строки
	BOOL		bRunCmd;	// Признак SQL-команды
	BOOL		bEvChanged;	// Признак OnChange
	CFldArray*	pArSrcFld;	// Данные источника
	CFldArray*	pArDstFld;	// Данные приемника
} ROW_INFO;

// Определение буфера строк данных
typedef CArray<ROW_INFO,ROW_INFO&> CBufArray;

// Описание команды
typedef struct _CMD_INFO {
	EV_TYPE			iEvent;	// ID события
	CString			sEvent;	// Наименование
	CString			sField;	// Поле БД
	CString			sCmd;	// Строка команд
	int				nCount;	// Число подкоманд
	CStringArray*	pArCmd;	// Массив подкоманд
	CArFldArray*	pArPrm;	// Массив параметров
	SQLHSTMT		hstmt;	// SQL HSTMT
} CMD_INFO;

// Определение массива команд
typedef CArray<CMD_INFO,CMD_INFO&> CCmdArray;

// Информация о профиле
typedef struct _PPL_INFO {
	CString		sParm;	// Параметры
	CString		sSrc;	// Источник
	CString		sDst;	// Приемник
	CTrnArray	arTrn;	// Массив трансфертов
} PPL_INFO;

// Информация о типе данных ODBC
typedef struct _TYPE_INFO {
	CString	sTypeName;	// SQL Type Name
	CString	sLTypeName;	// Local Type Name
	CString	sLitPfx;	// Literal Prefix
	CString	sLitSfx;	// Literal Suffix
	CString	sCrtParams;	// Create Params
	UINT	fSearchable;// Searchable Flag
	UINT	iType;		// SQL Data Type
	UINT	iMinScale;	// Minimum Scale
	UINT	iMaxScale;	// Maximim Scale
	ULONG	lPrec;		// Precision
	BOOL	bNullable;	// Is Nullable
	BOOL	bCaseSens;	// Is Case Sensitive
	BOOL	bUnsigned;	// Is Unsigned
	BOOL	bFxPrcScale;// Is Fixed Prec Scale
	BOOL	bAutoInc;	// Is Auto Increment
} TYPE_INFO;

// Определение массива типов данных ODBC
typedef CArray<TYPE_INFO,TYPE_INFO&> CTypeArray;

// Информация об источнике ODBC
typedef struct _ODBC_INFO {
	// Connection Handle
	SQLHDBC	hdbc;			// Connection Handle
	CString	sUserName;		// User Name
	// Driver Information
	CString	sDbmsName;		// DBMS Name
	CString	sDbmsVer;		// DBMS Verision
	CString	sDrvName;		// Driver Name
	CString	sDrvVer;		// Driver Version
	CString	sOdbcVer;		// ODBC Version
	CString	sDrvOdbcVer;	// Driver ODBC Version
	UWORD	iApiConf;		// ODBC API Conformance
	UWORD	iSqlConf;		// ODBC SQL Conformance
	BOOL	bSqlOptIef;		// ODBC SQL Opt IEF
	UWORD	iSagCliConf;	// ODBC Sag Cli Conformance
	UWORD	iActConns;		// Active Connections
	UWORD	iActStmts;		// Active Statements
	UWORD	iFileUsage;		// File Usage
	BOOL	bMultSets;		// Mult Result Sets
	BOOL	bReadOnly;		// Data Source Read Only
	BOOL	bAcsProc;		// Accessible Procedures
	BOOL	bAcsTabl;		// Accessible Tables
	// Limits
	UDWORD	lMaxBinLitLen;	// Max Binary Literal Length
	UDWORD	lMaxChrLitLen;	// Max Char Literal Length
	UWORD	iMaxColNameLen;	// Max Column Name Length
	UWORD	iMaxColInGrpBy;	// Max Columns In Group By
	UWORD	iMaxColInIndex;	// Max Columns In Index
	UWORD	iMaxColInOrdBy;	// Max Columns In Order By
	UWORD	iMaxColInSelect;// Max Columns In Select
	UWORD	iMaxColInTable;	// Max Columns In Table
	UWORD	iMaxCurNameLen;	// Max Cursor Name Length
	UDWORD	lMaxIndexSize;	// Max Index Size
	UWORD	iMaxOwnNameLen;	// Max Owner Name Length
	UWORD	iMaxPrcNameLen;	// Max Procedure Name Length
	UWORD	iMaxQlfNameLen;	// Max Qualifier Name Length
	UDWORD	lMaxRowSize;	// Max Row Size
	BOOL	bMaxRowSzIncL;	// Max Row Size Includes Long
	UDWORD	lMaxStmtLen;	// Max Statement Length
	UWORD	iMaxTabNameLen;	// Max Table Name Length
	UWORD	iMaxTabInSelect;// Max Tables In Select
	UWORD	iMaxUsrNameLen;	// Max User Name Length
	// Supported SQL
	BOOL	bAlterAddCol;	// Alter Table Add Column
	BOOL	bAlterDropCol;	// Alter Table Drop Column
	BOOL	bColAlias;		// Column Alias
	UWORD	iCorrName;		// Correlation Name
	BOOL	bDescParam;		// Describe Parameter
	BOOL	bProcColumns;	// Procedure Columns
	BOOL	bExpInOrdBy;	// Expressions In Order By
	UWORD	iGroupBy;		// Group By
	BOOL	bLikeEsc;		// Like Escape Clause
	BOOL	bOrdByInSel;	// Order By Columns In Select
	CString	sOuterJoins;	// Outer Joins
	BOOL	bPsPosDelete;	// Positioned Delete
	BOOL	bPsPosUpdate;	// Positioned Update
	BOOL	bPsSelForUpdate;// Positioned Select For Update
	BOOL	bSqCorrSubQry;	// SQ Correlated Subqueries
	BOOL	bSqComparison;	// SQ Comparison
	BOOL	bSqExists;		// SQ Exists
	BOOL	bSqIn;			// SQ In
	BOOL	blSqQuantif;	// SQ Quantified
	BOOL	bUUnion;		// Union
	BOOL	bUUnionAll;		// Union All
	// Miscellaneous
	UWORD	iTxnCapable;	// Txn Capable
	UWORD	iCurComBhvr;	// Cursor Commit Behavior
	UWORD	iCurRlbBhvr;	// Cursor Rollback Behavior
	BOOL	bTxnReadComm;	// Txn Read Commited
	BOOL	bTxnReadUncom;	// Txn Read Uncommited
	BOOL	bTxnRepRead;	// Txn Repeatable Read
	BOOL	bTxnSerial;		// Txn Serializable
	UDWORD	lDefTxnIsol;	// Default Txn Isolation
	BOOL	bMultActTxn;	// Multiple Active Txn
	BOOL	bBpClose;		// Bookmark Persistence Close
	BOOL	bBpDelete;		// Bookmark Persistence Delete
	BOOL	bBpDrop;		// Bookmark Persistence Drop
	BOOL	bBpScroll;		// Bookmark Persistence Scroll
	BOOL	bBpTrans;		// Bookmark Persistence Transaction
	BOOL	bBpUpdate;		// Bookmark Persistence Update
	BOOL	bBpOtherHstmt;	// Bookmark Persistence Other HSTMT
	BOOL	bLckNoChange;	// Lock No Change
	BOOL	bLckExclusive;	// Lock Exclusive
	BOOL	bLckUnlock;		// Lock Unlock
	BOOL	bSsAdditions;	// Static Sensitivity Additions
	BOOL	bSsDeletions;	// Static Sensitivity Deletions
	BOOL	bSsUpdates;		// Static Sensitivity Updates
	// Others
	BOOL	bNeedLDataLen;	// Need Long Data Length
	UWORD	iNonNullCols;	// Non Nullable Columns
	UWORD	iConcNullBhvr;	// Concat Null Behavior
	UWORD	iNullCollat;	// Null Collation
	BOOL	bProcedures;	// Procedured
	BOOL	bRowUpdates;	// Row Updates
	BOOL	bFdFetchNext;	// Fetch Direction Next
	BOOL	bFdFetchFirst;	// Fetch Direction First
	BOOL	bFdFetchLast;	// Fetch Direction Last
	BOOL	bFdFetchPrior;	// Fetch Direction Prior
	BOOL	bFdFetchAbs;	// Fetch Direction Absolute
	BOOL	bFdFetchRltv;	// Fetch Direction Relative
	BOOL	bFdFetchBmrk;	// Fetch Direction Bookmark
	BOOL	bGdAnyColumn;	// Get Data Any Column
	BOOL	bGdAnyOrder;	// Get Data Any Order
	BOOL	bGdBlock;		// Get Data Block
	BOOL	bGdBound;		// Get Data Bound
	BOOL	bPosPosition;	// Positioned Position
	BOOL	bPosRefresh;	// Positioned Refresh
	BOOL	bPosUpdate;		// Positioned Update
	BOOL	bPosDelete;		// Positioned Delete
	BOOL	bPosAdd;		// Positioned Add
	BOOL	bSccoReadOnly;	// Scroll Concurrency Read Only
	BOOL	bSccoLock;		// Scroll Concurrency Lock
	BOOL	bSccoOptRow;	// Scroll Concurrency Opt Rowver
	BOOL	bSccoOptVal;	// Scroll Concurrency Opt Values
	BOOL	bSoForwOnly;	// Scroll Options Forward Only
	BOOL	bSoStatic;		// Scroll Options Static
	BOOL	bSoKeyDriven;	// Scroll Options Keyset Driven
	BOOL	bSoDynamic;		// Scroll Options Dynamic
	BOOL	bSoMixed;		// Scroll Options Mixed
	BOOL	bOuDmlStmts;	// Owner Usage DML Statements
	BOOL	bOuProcInv;		// Owner Usage Procedure Invocation
	BOOL	bOuTableDef;	// Owner Usage Table Definition
	BOOL	bOuIndexDef;	// Owner Usage Index Definition
	BOOL	bOuPrivDef;		// Owner Usage Privilege Definition
	BOOL	bQuDmlStmts;	// Qualifier Usage DML Statements
	BOOL	bQuProcInv;		// Qualifier Usage Procedure Invocation
	BOOL	bQuTableDef;	// Qualifier Usage Table Definition
	BOOL	bQuIndexDef;	// Qualifier Usage Index Definition
	BOOL	bQuPrivDef;		// Qualifier Usage Privilege Definition
	// Type Array
	CTypeArray	arType;		// Array of Data Types
} ODBC_INFO;


/////////////////////////////////////////////////////////////////////
// Глобальные переменные приложения
//

extern HWND				hwndPopup;	// HWND Popup Window
extern HINSTANCE        hLangDLL;	// HINSTANCE Language DLL
extern CStdioFile       fileLog;	// Файл протокола
extern CStdioFile       fileMsg;	// Файл сообщений
extern CString          sFileLog;	// Наименование файла протокола
extern CString          sFileMsg;	// Наименование файла сообщений
extern CString          sLicense;	// Содержимое файла лицензии
extern BOOL             bFileLog;	// Признак файла протокола
extern BOOL             bFileMsg;	// Признак файла сообщений
extern BOOL             bExchange;	// Признак запуска обмена
extern BOOL             bImport;	// Признак импорта
extern BOOL             bExport;	// Признак экспорта
extern BOOL             bCancel;	// Признак останова
extern CMenu            menuMDI;	// Меню для всех MDI окон
extern TRN_PARM         TrnParm;	// Параметры Трансферта
extern CArray<PPL,PPL&> arPPL;		// Массив профилей данных

////////////////////////////////////////////////////////////////////
// Собщение в системный Tray

BOOL TrayMessage(HWND hWnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR szTip);

////////////////////////////////////////////////////////////////////
// Открытие файла протокола

BOOL OpenFileLog();

////////////////////////////////////////////////////////////////////
// Закрытие файла протокола

BOOL CloseFileLog();

/////////////////////////////////////////////////////////////////////
// Обработка таймера инициализации обмена (Timer #1)

void CALLBACK TimerExchange(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

/////////////////////////////////////////////////////////////////////
// Обработка таймера опроса файла обмена (Timer #2)

void CALLBACK TimerInterrogate(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

/////////////////////////////////////////////////////////////////////
// Запуск обмена данными

void StartExchange();

/////////////////////////////////////////////////////////////////////
// Останов обмена данными

void StopExchange();

/////////////////////////////////////////////////////////////////////
// Импорт данных из каталога обмена

BOOL ImportDataExchange(HWND hWnd);

/////////////////////////////////////////////////////////////////////
// Экспорт данных в каталог обмена

BOOL ExportDataExchange(HWND hWnd);

/////////////////////////////////////////////////////////////////////
// Инициализация установок Трансферта

BOOL InitOptions(void);

/////////////////////////////////////////////////////////////////////
// Запись установок Трансферта

BOOL WriteOptions(void);

/////////////////////////////////////////////////////////////////////
// Чтение из файла

BOOL FileGet(LPCTSTR szName, CString &strData);

/////////////////////////////////////////////////////////////////////
// Запись в файл

BOOL FilePut(LPCTSTR szName, CString strData);

/////////////////////////////////////////////////////////////////////
// Найти MDI окно по заголовку

CMDIChildWnd* FindMDIChildWnd(CMDIFrameWnd *pFrameWnd, LPCTSTR szTitle);

/////////////////////////////////////////////////////////////////////
// Поиск индекса профиля по имени файла

UINT FindPPL(LPCTSTR szFile);

/////////////////////////////////////////////////////////////////////
// Получить текст ошибки по идентификатору

BSTR GetErrMsg(ER_TYPE iErrType, LPCTSTR szErr = NULL);

/////////////////////////////////////////////////////////////////////
// Значение по ключевому слову

BOOL GetKeyVal(LPCTSTR szText, LPCTSTR szKey, CString &strData);

/////////////////////////////////////////////////////////////////////
// Значение по ключевому слову: Key=Value
// Вызов: GetKeyValue(input_string, "Key")

CString GetKeyValue(LPCTSTR szData, LPCTSTR szKey);

/////////////////////////////////////////////////////////////////////
// Значение строки по ключевому слову

BOOL GetKeyStr(LPCTSTR szText, LPCTSTR szKey, CString &strData);

/////////////////////////////////////////////////////////////////////
// Получить лексему с указанной позиции

int GetLex(LPCTSTR szText, int nStart, CString &strLex);

/////////////////////////////////////////////////////////////////////
// Выделить слово с указанной позиции

int GetWord(LPCTSTR szText, int nStart, CString &strWord);

/////////////////////////////////////////////////////////////////////
// Заполнение массива параметров

BOOL FillParmArray(LPCTSTR szText, CFldArray &arPrm);

/////////////////////////////////////////////////////////////////////
// Заполнение массива полей

BOOL FillFieldArray(LPCTSTR szText, CFldArray &arFld);

/////////////////////////////////////////////////////////////////////
// Заполнение массива команд

BOOL FillCmdArray(LPCTSTR szText, CCmdArray &arCmd);

/////////////////////////////////////////////////////////////////////
// Заполнение структуры описания трансферта

BOOL FillTrnInfo(LPCTSTR szText, TRN_INFO &trnInfo, CString &strErr);

/////////////////////////////////////////////////////////////////////
// Разбор выражения и замена ссылок на '?'

BOOL ParseRefExp(CString &strExp, CString &strErr);

/////////////////////////////////////////////////////////////////////
// Чтение определения профиля

BOOL ReadPPL(LPCTSTR szFile, PPL_INFO &info);

/////////////////////////////////////////////////////////////////////
// Запись определения профиля

BOOL WritePPL(LPCTSTR szFile, CString strName, PPL_INFO &info);

/////////////////////////////////////////////////////////////////////
// Запуск передачи данных с выбором профиля

BOOL RunPPLSel(int nKind = -1);

/////////////////////////////////////////////////////////////////////
// Запуск передачи данных

BOOL RunPipe(LPCTSTR szFile, LPCTSTR szTitle = NULL, 
			 int nKind = 0, BOOL bUseDlg = TRUE, 
			 LPCTSTR szParm = NULL);

/////////////////////////////////////////////////////////////////////
// SQL функции - помощники

UDWORD SQLGetError(SQLHSTMT hstmt, CString &strErr);

CString SQLGetInfoString(SQLHDBC hdbc, UWORD fInfoType);

UWORD SQLGetInfoShort(SQLHDBC hdbc, UWORD fInfoType);

UDWORD SQLGetInfoLong(SQLHDBC hdbc, UWORD fInfoType);

BOOL SQLGetInfoBool(SQLHDBC hdbc, UWORD fInfoType);

int SQLGetTypeID(CString strType);

int SQLGetTypeIndex(ODBC_INFO &info, UINT iSQLType);

TYPE_INFO SQLGetTypeDef(ODBC_INFO &info, UINT iSQLType);

BOOL SQLConvertType(ODBC_INFO &dstInfo, UINT iSrcType, UINT &iDstType);

CString SQLGetDefStrVal(UINT iSQLType);

CString SQLGetTypeStr(UINT iSQLType);

/////////////////////////////////////////////////////////////////////
// Заполнение информации об источнике ODBC

void SQLGetODBCInfo(SQLHDBC hdbc, ODBC_INFO& info);

/////////////////////////////////////////////////////////////////////
// Чтение строкового значения ключа из реестра

BOOL QueryRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, CString &sData, LPCTSTR szDefault = NULL);

/////////////////////////////////////////////////////////////////////
// Чтение числового значения ключа из реестра

BOOL QueryRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT &nData, UINT nDefault = 0);

/////////////////////////////////////////////////////////////////////
// Запись строкового значения ключа в реестр

BOOL WriteRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, LPCTSTR szData);

/////////////////////////////////////////////////////////////////////
// Запись числового значения ключа в реестр

BOOL WriteRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT nData);

/////////////////////////////////////////////////////////////////////
//
// Функции-помощники по защите программы
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Получение времени создания файла

BOOL _getFileCreateDT(LPCTSTR lpFileName, FILETIME &time);

/////////////////////////////////////////////////////////////////////
// Получение последнего времени доступа к файлу

BOOL _getFileAccessDT(LPCTSTR lpFileName, FILETIME &time);

/////////////////////////////////////////////////////////////////////
// Получение текущего системного времени "неявно" по файлу реестра

BOOL _getCurrentSysDT(FILETIME &time);

/////////////////////////////////////////////////////////////////////
// CTrans30App:
// See Trans30.cpp for the implementation of this class
//

class CTrans30App : public CWinApp
{
public:
	CTrans30App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrans30App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CTrans30App)
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// CPadDoc document

class CPadDoc : public CRichEditDoc
{
protected:
	CPadDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPadDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadDoc)
	public:
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPadDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPadDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////
// CPadView view

class CPadView : public CRichEditView
{
protected:
	CPadView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPadView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadView)
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPadView();

	// Generated message map functions
protected:
	CRect m_rectMargin;
	int   m_nUnits;
	BOOL SetDefaultMargins();
	BOOL SetDefaultFont();
	virtual void DeleteContents();
	//{{AFX_MSG(CPadView)
	afx_msg void OnFilePrint();
	afx_msg void OnPageSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	inline int roundleast(int n)
	{
		int mod = n%10;
		n -= mod;
		if (mod >= 5)
			n += 10;
		else if (mod <= -5)
			n -= 10;
		return n;
	}
	void RoundRect(LPRECT r1);
	void MulDivRect(LPRECT r1, LPRECT r2, int num, int div);
};

/////////////////////////////////////////////////////////////////////
// CPadFrame frame

class CPadFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPadFrame)
protected:
	CPadFrame();           // protected constructor used by dynamic creation

protected:
	static CMenu menu;

	// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPadFrame();

	// Generated message map functions
	//{{AFX_MSG(CPadFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDataClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRANS30_H)
