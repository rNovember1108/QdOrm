#pragma once

#include <QSharedPointer>
#include <QList>
#include "DBBase.h"
#include "ModuleBase/Database/Orm/QdOrmDao.h"
#include "ModuleBase/Database/Orm/QdOrmWhere.h"

namespace Sql
{
    class DBBase;
    template<typename T> class DmlBase
    {
    public:
        DmlBase() {}
        virtual ~DmlBase() {}

        /** 指定数据库 */
        void SetDatabase(QSharedPointer<DBBase> database)
        {
            Q_ASSERT_X(database, "SetDatabase()", "database is null");
            m_database = database;
        }

        /** 执行SQL语句 */
        bool ExecuteSql(const QString& sql, QSqlQuery& query, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::execute_sql<T>(sql, query, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 查找（主键匹配） */
        bool Exist(QSharedPointer<T>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            if(!Orm::exist(t, &sqlDatabase))
            {
                error = "data object not exists";
                return false;
            }
            return true;
        }

        /** 保存（插入或更新） */
        bool Save(QSharedPointer<T>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::save(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 保存集合（插入或更新） */
        bool SaveList(QList<QSharedPointer<T>>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::save(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 插入 */
        bool Add(QSharedPointer<T>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::insert(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 插入集合 */
        bool AddList(QList<QSharedPointer<T>>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::insert(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 查询（主键匹配）*/
        bool FindById(QSharedPointer<T>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::fetch_by_id(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 查询（条件匹配）*/
        bool FindByQuery(QList<QSharedPointer<T>>& t, Orm::QdOrmWhere& query, QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::fetch_by_query(t, &sqlDatabase, query);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 获取全部 */
        bool ListAll(QList<QSharedPointer<T>>& t, QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::fetch_by_query(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 统计数量（条件匹配） */
        bool CountByQuery(long long& size, const Orm::QdOrmWhere& query, const QString& column = QString(), QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::count<T>(size, &sqlDatabase, column, query);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 统计全部 */
        bool CountAll(long long& size, const QString& column = QString(), QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::count<T>(size, &sqlDatabase, column, Orm::QdOrmWhere());
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 求和（条件匹配） */
        bool SumByQuery(QVariant& sum, const Orm::QdOrmWhere& query, const QString& column = QString(), QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::sum<T>(sum, &sqlDatabase, column, query);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 更新（主键匹配）*/
        bool UpdateById(QSharedPointer<T>& t, const QStringList& columns = QStringList(), QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::update_by_id(t, &sqlDatabase, columns);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 更新（条件匹配） */
        bool UpdateByQuery(QSharedPointer<T>& t, const QStringList& columns,
            Orm::QdOrmWhere& query, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::update_by_query(t, &sqlDatabase, columns, query);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 更新集合（主键匹配） */
        bool UpdateList(QList<QSharedPointer<T>>& t, QString& error = QString(), int timeout = -1)
        {
            if(!m_database)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QStringList columns;
            QSqlError sqlError = Orm::update_by_id(t, &sqlDatabase, columns);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 主键删除*/
        bool DeleteById(QSharedPointer<T> t, QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError;
            sqlError = Orm::delete_by_id(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 主键批量删除*/
        bool DeleteListById(QList<QSharedPointer<T>>& t, QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError;
            sqlError = Orm::delete_by_id(t, &sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 条件删除 */
        bool DeleteByQuery(const Orm::QdOrmWhere& query, QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::delete_by_query<T>(&sqlDatabase, query);
            error = sqlError.text();
            return !sqlError.isValid();
        }

        /** 删除全部 */
        bool DeleteAll(QString& error = QString(), int timeout = -1)
        {
            if(m_database == nullptr)
            {
                error = ORM_DATABASE_IS_NOT_SPECIFIED_STRING;
                return false;
            }

            auto sqlDatabase = m_database->GetSqlDatabase();
            if(!sqlDatabase.isOpen())
            {
                error = ORM_DATABASE_IS_NOT_OPENED_STRING;
                return false;
            }

            QSqlError sqlError = Orm::delete_by_query<T>(&sqlDatabase);
            error = sqlError.text();
            return !sqlError.isValid();
        }

    private:
        QSharedPointer<DBBase> m_database;
    };
}