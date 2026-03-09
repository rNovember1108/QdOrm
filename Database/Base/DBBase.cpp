#include "DBBase.h"
#include <QReadLocker>
#include <QWriteLocker>
#include "ModuleBase/Log/Log.h"

namespace Sql
{
    void DBBase::SetDatabaseInfo(const DatabaseInfo& info)
    {
        m_info = info;
    }

    QReadWriteLock& DBBase::GetLock()
    {
        return m_lock;
    }

    QString DBBase::GetName() const
    {
        return m_info.databaseName;
    }

    QSqlDatabase DBBase::GetSqlDatabase()
    {
        Qt::HANDLE tid = QThread::currentThreadId();
        if(!m_connectionTable.contains(tid))
            Connect();

        QString connectionName = m_connectionTable[tid];
        if(!QSqlDatabase::contains(connectionName))
            return QSqlDatabase();
        else
            return QSqlDatabase::database(connectionName);
    }

    bool DBBase::CheckIntegrity(const QSqlDatabase& database)
    {
        QSqlQuery query(database);
        if(query.exec("PRAGMA integrity_check;"))
        {
            if(query.next())
            {
                const QString result = query.value(0).toString().toLower();
                return (result == "ok");
            }
        }
        return false;
    }

    bool DBBase::Connect()
    {
        Qt::HANDLE tid = QThread::currentThreadId();
        QString connectionName = QUuid::createUuid().toString();
        QSqlDatabase database = QSqlDatabase::addDatabase(m_info.databaseType, connectionName);
        database.setDatabaseName(m_info.databaseName);

        if(m_info.supportCipher)
        {
            database.setPassword(m_info.databasePwd);
            if(!QFile::exists(m_info.databaseName))
                database.setConnectOptions("QSQLITE_CREATE_KEY");
        }
        
        if(!database.open())
        {
            LOG_ERROR(QStringLiteral("open database failed，name:%1，reason:%2")
                .arg(m_info.databaseName)
                .arg(database.lastError().text()).toStdString());
            return false;
        }

        // 检查数据库完整性
        if(!CheckIntegrity(database))
        {
            LOG_ERROR(QStringLiteral("check database integrity failed，name:%1")
                .arg(m_info.databaseName).toStdString());
            database.close();
            return false;
        }

        // 缓存数据库连接
        m_connectionTable[tid] = connectionName;

        // 设置SQLITE
        QSqlQuery query(database);
        query.exec("PRAGMA journal_mode = PERSIST;");
        //query.exec("PRAGMA synchronous = OFF;");

        return true;
    }

    void DBBase::Disconnect()
    {
        QWriteLocker locker(&m_lock);

        // 确保remove前，引用计数归零
        {
            auto database = GetSqlDatabase();
            if(database.isOpen())
                database.close();
        }
        for(auto& i : m_connectionTable)
        {
            QSqlDatabase::removeDatabase(i);
        }
    }

    void DBBase::ReleaseConnect()
    {
        Qt::HANDLE tid = QThread::currentThreadId();
        if(m_connectionTable.contains(tid))
        {
            auto connectionName = m_connectionTable[tid];
            m_connectionTable.remove(tid);

            QWriteLocker locker(&m_lock);
            {
                if(QSqlDatabase::contains(connectionName))
                {
                    auto database = QSqlDatabase::database(connectionName);
                    if(database.isOpen())
                    {
                        database.close();
                    }
                }
            }
            QSqlDatabase::removeDatabase(connectionName);
        }
    }

    bool DBBase::IsConnectionTableEmpty()
    {
        return m_connectionTable.isEmpty();
    }

    bool DBBase::CreateTable()
    {
        return false;
    }

    bool DBBase::ExecuteSql(const QString& sql, QSqlQuery& query, QString& error)
    {
        QWriteLocker locker(&m_lock);
        QSqlDatabase database = GetSqlDatabase();
        if(!database.isOpen())
        {
            error = "database is not opened";
            return false;
        }

        QSqlQuery temp(database);
        if(!temp.exec(sql))
        {
            error = temp.lastError().text();
            return false;
        }

        query = temp;
        return true;
    }
}
