from supabase import create_client, Client


class SupabaseClient:
    SUPABASE_URL = "https://jaqghporpakiqmktsrir.supabase.co"
    ANON_KEY = (
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InV4c2NzcmhvaGd5Zm94dHdkaXl4Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3Mjg0MTM2MzEsImV4cCI6MjA0Mzk4OTYzMX0.632pdeTGRvqn_qAl3gWV4zonQ1inUzaiFVpu1IUnMQ0"
    )
    SERVICE_ROLE_KEY = (
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InV4c2NzcmhvaGd5Zm94dHdkaXl4Iiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTcyODQxMzYzMSwiZXhwIjoyMDQzOTg5NjMxfQ.4CORmDY_dzOtzSv1ulugMajE7dU9yrDMGpZrg4Mu4zw"
    )

    def __init__(self):
        self.supabase: Client = create_client(self.SUPABASE_URL, self.ANON_KEY)

    def validate_response(self, response):
        if hasattr(response, "data"):
            return response.data
        elif hasattr(response, "error"):
            raise Exception(f"Supabase error: {response.error['message']}")
        else:
            raise Exception(f"Unexpected response format: {response}")

    def fetch_all(self, table: str):
        response = self.supabase.table(table).select("*").execute()
        if hasattr(response, "data"):
            return response.data
        elif hasattr(response, "error"):
            raise Exception(f"Error fetching data: {response.error}")
        else:
            raise Exception(f"Unexpected response format: {response}")

    def insert_row(self, table: str, row: dict):
        response = self.supabase.table(table).insert(row).execute()
        if hasattr(response, "data"):
            return response.data
        elif hasattr(response, "error"):
            raise Exception(f"Error inserting row: {response.error}")
        else:
            raise Exception(f"Unexpected response format: {response}")

    def update_row(self, table: str, conditions: dict, updates: dict):
        query = self.supabase.table(table).update(updates)
        for key, value in conditions.items():
            query = query.eq(key, value)
        response = query.execute()
        return self.validate_response(response)

    def delete_row(self, table: str, conditions: dict):
        query = self.supabase.table(table).delete()
        for key, value in conditions.items():
            query = query.eq(key, value)
        response = query.execute()
        return self.validate_response(response)

    def upsert_row(self, table: str, row: dict, conflict_columns: list):
        response = self.supabase.table(table).upsert(row, on_conflict=conflict_columns).execute()
        return self.validate_response(response)

    def bulk_insert(self, table: str, rows: list):
        response = self.supabase.table(table).insert(rows).execute()
        return self.validate_response(response)

    def fetch_specific(self, table: str, column: str, condition: dict):
        query = self.supabase.table(table).select(column)
        for key, value in condition.items():
            query = query.eq(key, value)

        response = query.execute()
        data = self.validate_response(response)

        if data:
            return data[0].get(column, None)
        else:
            return None
