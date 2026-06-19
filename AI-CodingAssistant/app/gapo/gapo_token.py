import requests
import os
import time
from app.common.config import logger

class GapoAuthClient:
    _instance = None
    _access_token = None
    _expires_at = 0
    _refresh_token = None
    _base_url = None
    


    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(GapoAuthClient, cls).__new__(cls)
            cls._base_url = os.environ.get("GAPO_AUTH_API_URL")
        return cls._instance


    def check_identifier(cls, company_name: str, identifier_code: str):
        """
        Check the identifier code of a company in Gapo

        Args:
            company_name (str): The company name
            identifier_code (str): The identifier code

        Returns:
            dict: The response from Gapo
        """
        url = f"{cls._base_url}/check-identifier-code"
        headers = {"Content-Type": "application/json"}
        payload = {"company_name": company_name, "identifier_code": identifier_code}
        response = requests.post(url, json=payload, headers=headers)
        return response.json()

    def get_access_token(cls, 
              device_id: str = os.environ.get("GAPO_AUTH_DEVICE_ID"), 
              client_id: str = os.environ.get("GAPO_AUTH_CLIENT_ID"), 
              trusted_device: bool = bool(int(os.environ.get("GAPO_AUTH_TRUSTED_DEVICE") or 0)), 
              password: str = os.environ.get("GAPO_AUTH_PASSWORD"), 
              company_name: str = os.environ.get("GAPO_AUTH_COMPANY_NAME"), 
              identifier_code: str = os.environ.get("GAPO_AUTH_IDENTIFIER_CODE")
              ) -> str:
        """
        This method gets the access token from Gapo

        Args:
            device_id (str): The device id
            client_id (str): The client id
            trusted_device (bool): The trusted device
            password (str): The password
            company_name (str): The company name
            identifier_code (str): The identifier code
        
        Returns:
            str: The access token from Gapo
        """
        if os.environ.get("ENV") == "dev":
            env_access_token = os.environ.get("GAPO_ACCESS_TOKEN", "")
            if len(env_access_token) > 0 and cls.check_access_token(env_access_token):
                logger.debug("Using access token from environment variable.")
                return env_access_token

        
        if cls._access_token and cls._expires_at > int(time.time() + 100):
            return cls._access_token
        url = f"{cls._base_url}/login"
        payload = {
            "device_id": device_id,
            "client_id": client_id,
            "trusted_device": trusted_device,
            "password": password,
            "company_name": company_name,
            "identifier_code": identifier_code
        }
        try:
            headers = {"Content-Type": "application/json"}
            response = requests.post(url, json=payload, headers=headers)
            if response.status_code == 200:
                cls._access_token = response.json()['data']['access_token']
                cls._expires_at = response.json()['data']['access_token_expires_at']
                cls._refresh_token = response.json()['data']['refresh_token']
                logger.debug("Succesfully got access token from Gapo")
                logger.debug(f"Access token: {cls._access_token}")
                return cls._access_token
            else:
                raise requests.RequestException(f"Failed to get access token from Gapo! \
                                                Status code: {response.status_code}, Message: {response.json()}")
                
        except Exception as e:
            raise requests.RequestException(f"Failed to get access token from Gapo! {e}")


    def check_access_token(cls, access_token: str) -> bool:
        """
        Check the access token

        Args:
            access_token (str): The access token

        Returns:
            bool: True if the access token is valid, False otherwise
        """
        try:
            if access_token is None or len(access_token) == 0:
                return False

            url = os.environ.get("GAPO_BASE_API_URL") + "messages"
            headers = {"Content-Type": "application/json", "Authorization": f"Bearer {access_token}"}
            response = requests.post(url, headers=headers)
            if response.status_code == 401:
                return False
            return True
        except Exception as e:
            return False
    
tokenizer = GapoAuthClient()