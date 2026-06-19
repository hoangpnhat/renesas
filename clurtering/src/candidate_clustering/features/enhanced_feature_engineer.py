"""
Enhanced Feature Engineering Module - Phase 2 Improvements

Includes:
- Smart skill inference (forward + reverse with domain context)
- Domain-aware reverse inference with high confidence
- Confidence-based filtering
- All original features + inferred skills tracking
"""

from typing import Dict, List, Tuple, Optional, Set
from collections import defaultdict
import numpy as np
import json
from pathlib import Path

# Import original FeatureEngineer
from .feature_engineer import FeatureEngineer


class EnhancedFeatureEngineer(FeatureEngineer):
    """
    Enhanced version of FeatureEngineer with smart skill inference.
    
    Improvements:
    1. Forward inference: Specific → General (FastAPI → Python)
    2. Domain-aware reverse inference: General → Specific (Python + ML → pandas)
    3. Confidence tracking for all inferences
    4. Flexible confidence filtering
    """
    
    SKILL_DEPENDENCIES = {
        # Web Frameworks → Languages
        "django": {"python": 0.90},
        "fastapi": {"python": 0.90},
        "flask": {"python": 0.85},
        "express": {"javascript": 0.85, "node.js": 0.90},
        "express.js": {"javascript": 0.85, "node.js": 0.90},
        "spring": {"java": 0.90},
        "rails": {"ruby": 0.90},
        "laravel": {"php": 0.85},
        "asp.net": {"c#": 0.90, ".net": 0.85},
        "nest.js": {"typescript": 0.90, "node.js": 0.85},
        "gin": {"go": 0.90},
        "fiber": {"go": 0.85},
        
        # Frontend Frameworks → JavaScript/TypeScript
        "react": {"javascript": 0.85},
        "react.js": {"javascript": 0.85},
        "vue": {"javascript": 0.85},
        "vue.js": {"javascript": 0.85},
        "angular": {"typescript": 0.90, "javascript": 0.80},
        "next.js": {"react": 0.90, "javascript": 0.80},
        "svelte": {"javascript": 0.85},
        "nuxt.js": {"vue": 0.90, "javascript": 0.80},
        "remix": {"react": 0.85, "javascript": 0.80},
        "solid.js": {"javascript": 0.85},
        "backbone.js": {"javascript": 0.80},
        "ember.js": {"javascript": 0.80},
        
        # CSS Frameworks & Tools
        "tailwind": {"css": 0.85, "html": 0.75},
        "tailwindcss": {"css": 0.85, "html": 0.75},
        "bootstrap": {"css": 0.80, "html": 0.75},
        "sass": {"css": 0.85},
        "scss": {"css": 0.85},
        "less": {"css": 0.80},
        "styled-components": {"css": 0.80, "react": 0.75},
        "material-ui": {"react": 0.85, "css": 0.70},
        "mui": {"react": 0.85, "css": 0.70},
        
        # Mobile Development
        "react native": {"react": 0.85, "javascript": 0.80, "mobile development": 0.85},
        "flutter": {"dart": 0.90, "mobile development": 0.85},
        "swift": {"ios": 0.90, "mobile development": 0.85},
        "kotlin": {"android": 0.90, "mobile development": 0.85},
        "ionic": {"angular": 0.80, "mobile development": 0.75},
        "xamarin": {"c#": 0.85, "mobile development": 0.80},
        
        # Databases → SQL/NoSQL
        "postgresql": {"sql": 0.90, "database": 0.85},
        "mysql": {"sql": 0.90, "database": 0.85},
        "mongodb": {"nosql": 0.85, "database": 0.80},
        "redis": {"nosql": 0.75, "database": 0.70},
        "oracle": {"sql": 0.90, "database": 0.85},
        "mariadb": {"sql": 0.90, "database": 0.85},
        "sqlite": {"sql": 0.85, "database": 0.80},
        "cassandra": {"nosql": 0.85, "database": 0.80},
        "dynamodb": {"nosql": 0.80, "aws": 0.75, "database": 0.75},
        "couchdb": {"nosql": 0.80, "database": 0.75},
        "neo4j": {"graph database": 0.90, "database": 0.80},
        "elasticsearch": {"nosql": 0.80, "database": 0.75, "search": 0.85},
        "firestore": {"nosql": 0.80, "firebase": 0.85, "database": 0.75},
        
        # Cloud Services → Cloud Concepts
        "ec2": {"aws": 0.90, "cloud": 0.85},
        "s3": {"aws": 0.90, "cloud": 0.80},
        "lambda": {"aws": 0.85, "cloud": 0.80},
        "gke": {"gcp": 0.90, "kubernetes": 0.85, "cloud": 0.80},
        "azure": {"cloud": 0.85},
        "azure devops": {"azure": 0.85, "devops": 0.80, "ci/cd": 0.75},
        "cloudformation": {"aws": 0.85, "infrastructure as code": 0.80},
        "cloud functions": {"gcp": 0.85, "cloud": 0.80},
        "app engine": {"gcp": 0.85, "cloud": 0.80},
        "cloud run": {"gcp": 0.85, "docker": 0.75, "cloud": 0.80},
        "firebase": {"gcp": 0.80, "cloud": 0.75},
        "heroku": {"cloud": 0.80},
        "digitalocean": {"cloud": 0.75},
        "vercel": {"cloud": 0.75, "next.js": 0.70},
        "netlify": {"cloud": 0.75, "jamstack": 0.70},
        
        # DevOps Tools → DevOps Concepts
        "kubernetes": {"docker": 0.85, "devops": 0.80},
        "k8s": {"docker": 0.85, "devops": 0.80},
        "terraform": {"devops": 0.80, "infrastructure as code": 0.85},
        "jenkins": {"ci/cd": 0.85, "devops": 0.80},
        "docker": {"devops": 0.80, "containerization": 0.90},
        "docker-compose": {"docker": 0.85, "devops": 0.75},
        "ansible": {"devops": 0.80, "automation": 0.85},
        "puppet": {"devops": 0.80, "automation": 0.80},
        "chef": {"devops": 0.80, "automation": 0.80},
        "gitlab ci": {"ci/cd": 0.85, "devops": 0.80},
        "github actions": {"ci/cd": 0.85, "devops": 0.80},
        "circleci": {"ci/cd": 0.85, "devops": 0.80},
        "travis ci": {"ci/cd": 0.80, "devops": 0.75},
        "argocd": {"kubernetes": 0.85, "ci/cd": 0.80, "devops": 0.75},
        "helm": {"kubernetes": 0.85, "devops": 0.75},
        "prometheus": {"monitoring": 0.85, "devops": 0.75},
        "grafana": {"monitoring": 0.85, "devops": 0.75},
        "datadog": {"monitoring": 0.80, "devops": 0.75},
        "new relic": {"monitoring": 0.80, "devops": 0.75},
        "splunk": {"monitoring": 0.80, "logging": 0.85, "devops": 0.70},
        "elk stack": {"elasticsearch": 0.80, "logging": 0.85, "devops": 0.75},
        "nginx": {"web server": 0.85, "devops": 0.70},
        "apache": {"web server": 0.85, "devops": 0.70},
        
        # Data Science Tools → Python/Data Skills
        "pandas": {"python": 0.85, "data analysis": 0.80},
        "numpy": {"python": 0.85},
        "tensorflow": {"python": 0.85, "machine learning": 0.80, "deep learning": 0.85},
        "pytorch": {"python": 0.85, "machine learning": 0.80, "deep learning": 0.85},
        "scikit-learn": {"python": 0.80, "machine learning": 0.85},
        "sklearn": {"python": 0.80, "machine learning": 0.85},
        "keras": {"python": 0.85, "deep learning": 0.85, "tensorflow": 0.75},
        "matplotlib": {"python": 0.80, "data visualization": 0.85},
        "seaborn": {"python": 0.80, "matplotlib": 0.75, "data visualization": 0.85},
        "plotly": {"python": 0.75, "data visualization": 0.85},
        "jupyter": {"python": 0.80, "data science": 0.75},
        "scipy": {"python": 0.85, "scientific computing": 0.80},
        "opencv": {"python": 0.80, "computer vision": 0.85},
        "spacy": {"python": 0.80, "nlp": 0.85},
        "nltk": {"python": 0.80, "nlp": 0.85},
        "hugging face": {"python": 0.80, "nlp": 0.85, "transformers": 0.85},
        "transformers": {"python": 0.80, "nlp": 0.85, "deep learning": 0.80},
        "xgboost": {"python": 0.80, "machine learning": 0.85},
        "lightgbm": {"python": 0.80, "machine learning": 0.85},
        "catboost": {"python": 0.80, "machine learning": 0.85},
        
        # Big Data & Data Engineering
        "spark": {"scala": 0.75, "big data": 0.85, "data engineering": 0.80},
        "pyspark": {"python": 0.80, "spark": 0.85, "big data": 0.80},
        "hadoop": {"big data": 0.85, "data engineering": 0.80},
        "hive": {"sql": 0.80, "hadoop": 0.80, "big data": 0.75},
        "kafka": {"data streaming": 0.85, "big data": 0.75},
        "airflow": {"python": 0.80, "data engineering": 0.85, "workflow orchestration": 0.85},
        "dbt": {"sql": 0.85, "data engineering": 0.80},
        "snowflake": {"sql": 0.85, "data warehouse": 0.85, "cloud": 0.75},
        "redshift": {"sql": 0.85, "aws": 0.80, "data warehouse": 0.85},
        "bigquery": {"sql": 0.85, "gcp": 0.80, "data warehouse": 0.85},
        "databricks": {"spark": 0.85, "big data": 0.80, "data engineering": 0.80},
        
        # Testing Frameworks
        "jest": {"javascript": 0.85, "testing": 0.85},
        "mocha": {"javascript": 0.80, "testing": 0.85},
        "pytest": {"python": 0.85, "testing": 0.85},
        "unittest": {"python": 0.80, "testing": 0.85},
        "selenium": {"testing": 0.85, "automation": 0.80},
        "cypress": {"javascript": 0.80, "testing": 0.85, "e2e testing": 0.85},
        "playwright": {"javascript": 0.80, "testing": 0.85, "e2e testing": 0.85},
        "junit": {"java": 0.85, "testing": 0.85},
        "testng": {"java": 0.80, "testing": 0.85},
        "rspec": {"ruby": 0.85, "testing": 0.85},
        "jasmine": {"javascript": 0.80, "testing": 0.85},
        
        # API & Integration
        "graphql": {"api": 0.85, "javascript": 0.70},
        "rest api": {"api": 0.85, "http": 0.80},
        "grpc": {"api": 0.85, "protocol buffers": 0.80},
        "postman": {"api": 0.80, "testing": 0.70},
        "swagger": {"api": 0.80, "rest api": 0.75},
        "apollo": {"graphql": 0.85, "javascript": 0.70},
        "soap": {"api": 0.75, "xml": 0.75},
        
        # Version Control & Collaboration
        "git": {"version control": 0.90},
        "github": {"git": 0.85, "version control": 0.80},
        "gitlab": {"git": 0.85, "version control": 0.80},
        "bitbucket": {"git": 0.85, "version control": 0.80},
        "mercurial": {"version control": 0.80},
        "svn": {"version control": 0.75},
        
        # Security Tools
        "owasp": {"security": 0.85, "web security": 0.80},
        "burp suite": {"security": 0.85, "penetration testing": 0.80},
        "metasploit": {"security": 0.85, "penetration testing": 0.85},
        "wireshark": {"networking": 0.80, "security": 0.75},
        "nmap": {"networking": 0.80, "security": 0.75},
        "snyk": {"security": 0.80, "devops": 0.70},
        "sonarqube": {"code quality": 0.85, "security": 0.75},
        
        # Message Queues & Event Streaming
        "rabbitmq": {"message queue": 0.85, "distributed systems": 0.75},
        "activemq": {"message queue": 0.80, "java": 0.70},
        "zeromq": {"message queue": 0.80, "distributed systems": 0.75},
        "nats": {"message queue": 0.80, "distributed systems": 0.75},
        "pulsar": {"message queue": 0.80, "data streaming": 0.80},
        
        # Game Development
        "unity": {"c#": 0.85, "game development": 0.90},
        "unreal engine": {"c++": 0.85, "game development": 0.90},
        "godot": {"game development": 0.85, "gdscript": 0.80},
        "pygame": {"python": 0.80, "game development": 0.80},
        
        # Blockchain & Web3
        "solidity": {"blockchain": 0.90, "smart contracts": 0.90},
        "web3.js": {"javascript": 0.80, "blockchain": 0.85, "ethereum": 0.80},
        "ethers.js": {"javascript": 0.80, "blockchain": 0.85, "ethereum": 0.80},
        "truffle": {"solidity": 0.85, "blockchain": 0.80},
        "hardhat": {"solidity": 0.85, "blockchain": 0.80, "ethereum": 0.80},
        
        # Build Tools & Package Managers
        "webpack": {"javascript": 0.80, "build tools": 0.85},
        "vite": {"javascript": 0.80, "build tools": 0.85},
        "parcel": {"javascript": 0.75, "build tools": 0.80},
        "rollup": {"javascript": 0.80, "build tools": 0.85},
        "npm": {"javascript": 0.80, "node.js": 0.75, "package manager": 0.85},
        "yarn": {"javascript": 0.80, "node.js": 0.75, "package manager": 0.85},
        "pnpm": {"javascript": 0.80, "node.js": 0.75, "package manager": 0.85},
        "pip": {"python": 0.85, "package manager": 0.85},
        "maven": {"java": 0.85, "build tools": 0.85},
        "gradle": {"java": 0.85, "kotlin": 0.75, "build tools": 0.85},
        "composer": {"php": 0.85, "package manager": 0.85},
        "cargo": {"rust": 0.90, "package manager": 0.85},
    }
    
    # Domain-aware reverse inference (General → Specific with context)
    SKILL_REVERSE_INFERENCE = {
        "python": {
            "domain_skills": {
                "backend": {
                    "min_python_score": 70,
                    "frameworks": {
                        "django": {"score_ratio": 0.75, "confidence": 0.85},
                        "fastapi": {"score_ratio": 0.70, "confidence": 0.80},
                        "flask": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                        "sqlalchemy": {"score_ratio": 0.60, "confidence": 0.75},
                        "pytest": {"score_ratio": 0.55, "confidence": 0.70},
                    },
                    "infer_one_of": ["django", "fastapi", "flask"],
                    "infer_all": ["pytest", "rest api"],
                },
                "machine_learning": {
                    "min_python_score": 75,
                    "libraries": {
                        "pandas": {"score_ratio": 0.85, "confidence": 0.95},
                        "numpy": {"score_ratio": 0.85, "confidence": 0.95},
                        "scikit-learn": {"score_ratio": 0.75, "confidence": 0.85},
                        "matplotlib": {"score_ratio": 0.65, "confidence": 0.70},
                    },
                    "frameworks": {
                        "pytorch": {"score_ratio": 0.70, "confidence": 0.90},
                        "tensorflow": {"score_ratio": 0.70, "confidence": 0.88},
                    },
                    "tools": {
                        "jupyter": {"score_ratio": 0.55, "confidence": 0.75},
                    },
                    "infer_all": ["pandas", "numpy"],
                    "infer_one_of": ["pytorch", "tensorflow"],
                },
                "data_science": {
                    "min_python_score": 70,
                    "libraries": {
                        "pandas": {"score_ratio": 0.85, "confidence": 0.95},
                        "numpy": {"score_ratio": 0.80, "confidence": 0.90},
                        "matplotlib": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "jupyter": {"score_ratio": 0.65, "confidence": 0.80},
                        "data visualization": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["pandas", "jupyter"],
                },
                "data_engineering": {
                    "min_python_score": 75,
                    "frameworks": {
                        "pyspark": {"score_ratio": 0.75, "confidence": 0.85},
                        "airflow": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "libraries": {
                        "pandas": {"score_ratio": 0.80, "confidence": 0.90},
                    },
                    "tools": {
                        "sql": {"score_ratio": 0.75, "confidence": 0.85},
                        "etl": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_all": ["sql", "pandas"],
                    "infer_one_of": ["pyspark", "airflow"],
                },
                "automation": {
                    "min_python_score": 65,
                    "libraries": {
                        "selenium": {"score_ratio": 0.70, "confidence": 0.80},
                        "pytest": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "tools": {
                        "scripting": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "infer_all": ["scripting"],
                },
            }
        },
        
        "javascript": {
            "domain_skills": {
                "frontend": {
                    "min_javascript_score": 70,
                    "frameworks": {
                        "react": {"score_ratio": 0.75, "confidence": 0.85},
                        "vue": {"score_ratio": 0.70, "confidence": 0.80},
                        "angular": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "html": {"score_ratio": 0.70, "confidence": 0.90},
                        "css": {"score_ratio": 0.65, "confidence": 0.85},
                        "webpack": {"score_ratio": 0.50, "confidence": 0.70},
                    },
                    "infer_one_of": ["react", "vue", "angular"],
                    "infer_all": ["html", "css"],
                },
                "backend": {
                    "min_javascript_score": 70,
                    "frameworks": {
                        "node.js": {"score_ratio": 0.80, "confidence": 0.90},
                        "express": {"score_ratio": 0.70, "confidence": 0.85},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["node.js"],
                },
                "fullstack": {
                    "min_javascript_score": 75,
                    "frameworks": {
                        "next.js": {"score_ratio": 0.75, "confidence": 0.85},
                        "nuxt.js": {"score_ratio": 0.70, "confidence": 0.80},
                        "react": {"score_ratio": 0.70, "confidence": 0.85},
                        "node.js": {"score_ratio": 0.75, "confidence": 0.85},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.65, "confidence": 0.80},
                        "database": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_one_of": ["next.js", "nuxt.js"],
                    "infer_all": ["node.js", "rest api"],
                },
            }
        },
        
        "typescript": {
            "domain_skills": {
                "frontend": {
                    "min_typescript_score": 70,
                    "frameworks": {
                        "angular": {"score_ratio": 0.80, "confidence": 0.90},
                        "react": {"score_ratio": 0.75, "confidence": 0.85},
                        "vue": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "javascript": {"score_ratio": 0.85, "confidence": 0.95},
                        "html": {"score_ratio": 0.65, "confidence": 0.80},
                        "css": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["javascript"],
                    "infer_one_of": ["angular", "react", "vue"],
                },
                "backend": {
                    "min_typescript_score": 70,
                    "frameworks": {
                        "nest.js": {"score_ratio": 0.75, "confidence": 0.85},
                        "node.js": {"score_ratio": 0.80, "confidence": 0.90},
                    },
                    "tools": {
                        "javascript": {"score_ratio": 0.85, "confidence": 0.95},
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["javascript", "node.js"],
                },
            }
        },
        
        "java": {
            "domain_skills": {
                "backend": {
                    "min_java_score": 70,
                    "frameworks": {
                        "spring": {"score_ratio": 0.80, "confidence": 0.90},
                        "spring boot": {"score_ratio": 0.75, "confidence": 0.85},
                    },
                    "tools": {
                        "maven": {"score_ratio": 0.65, "confidence": 0.80},
                        "gradle": {"score_ratio": 0.60, "confidence": 0.75},
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                        "junit": {"score_ratio": 0.55, "confidence": 0.70},
                    },
                    "infer_one_of": ["spring", "spring boot"],
                    "infer_all": ["junit"],
                },
                "enterprise": {
                    "min_java_score": 75,
                    "frameworks": {
                        "spring": {"score_ratio": 0.85, "confidence": 0.90},
                        "hibernate": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "sql": {"score_ratio": 0.70, "confidence": 0.80},
                        "microservices": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_all": ["spring", "sql"],
                },
                "android": {
                    "min_java_score": 70,
                    "frameworks": {
                        "android sdk": {"score_ratio": 0.85, "confidence": 0.90},
                    },
                    "tools": {
                        "mobile development": {"score_ratio": 0.75, "confidence": 0.85},
                        "gradle": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["android sdk", "mobile development"],
                },
            }
        },
        
        "go": {
            "domain_skills": {
                "backend": {
                    "min_go_score": 70,
                    "frameworks": {
                        "gin": {"score_ratio": 0.75, "confidence": 0.85},
                        "fiber": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.65, "confidence": 0.80},
                        "microservices": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_one_of": ["gin", "fiber"],
                },
                "devops": {
                    "min_go_score": 65,
                    "tools": {
                        "docker": {"score_ratio": 0.70, "confidence": 0.80},
                        "kubernetes": {"score_ratio": 0.65, "confidence": 0.75},
                        "cloud": {"score_ratio": 0.60, "confidence": 0.70},
                    },
                    "infer_all": ["docker"],
                },
            }
        },
        
        "rust": {
            "domain_skills": {
                "systems_programming": {
                    "min_rust_score": 70,
                    "tools": {
                        "cargo": {"score_ratio": 0.85, "confidence": 0.90},
                        "systems programming": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "infer_all": ["cargo"],
                },
                "backend": {
                    "min_rust_score": 70,
                    "frameworks": {
                        "actix": {"score_ratio": 0.75, "confidence": 0.85},
                        "rocket": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_one_of": ["actix", "rocket"],
                },
            }
        },
        
        "c#": {
            "domain_skills": {
                "backend": {
                    "min_c#_score": 70,
                    "frameworks": {
                        ".net": {"score_ratio": 0.85, "confidence": 0.90},
                        "asp.net": {"score_ratio": 0.80, "confidence": 0.85},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                        "entity framework": {"score_ratio": 0.65, "confidence": 0.80},
                    },
                    "infer_all": [".net"],
                },
                "game_development": {
                    "min_c#_score": 70,
                    "frameworks": {
                        "unity": {"score_ratio": 0.85, "confidence": 0.90},
                    },
                    "tools": {
                        "game development": {"score_ratio": 0.80, "confidence": 0.85},
                        "3d graphics": {"score_ratio": 0.60, "confidence": 0.70},
                    },
                    "infer_all": ["unity", "game development"],
                },
            }
        },
        
        "php": {
            "domain_skills": {
                "backend": {
                    "min_php_score": 70,
                    "frameworks": {
                        "laravel": {"score_ratio": 0.80, "confidence": 0.85},
                        "symfony": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "composer": {"score_ratio": 0.70, "confidence": 0.80},
                        "mysql": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_one_of": ["laravel", "symfony"],
                    "infer_all": ["composer"],
                },
            }
        },
        
        "ruby": {
            "domain_skills": {
                "backend": {
                    "min_ruby_score": 70,
                    "frameworks": {
                        "rails": {"score_ratio": 0.85, "confidence": 0.90},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                        "rspec": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_all": ["rails", "rspec"],
                },
            }
        },
        
        "kotlin": {
            "domain_skills": {
                "android": {
                    "min_kotlin_score": 70,
                    "frameworks": {
                        "android sdk": {"score_ratio": 0.85, "confidence": 0.90},
                        "jetpack compose": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "mobile development": {"score_ratio": 0.80, "confidence": 0.85},
                        "gradle": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_all": ["android sdk", "mobile development"],
                },
                "backend": {
                    "min_kotlin_score": 70,
                    "frameworks": {
                        "spring boot": {"score_ratio": 0.75, "confidence": 0.85},
                        "ktor": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "tools": {
                        "rest api": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "infer_one_of": ["spring boot", "ktor"],
                },
            }
        },
        
        "swift": {
            "domain_skills": {
                "ios": {
                    "min_swift_score": 70,
                    "frameworks": {
                        "swiftui": {"score_ratio": 0.80, "confidence": 0.85},
                        "uikit": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "tools": {
                        "mobile development": {"score_ratio": 0.85, "confidence": 0.90},
                        "xcode": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "infer_all": ["mobile development", "xcode"],
                    "infer_one_of": ["swiftui", "uikit"],
                },
            }
        },
        
        "dart": {
            "domain_skills": {
                "mobile": {
                    "min_dart_score": 70,
                    "frameworks": {
                        "flutter": {"score_ratio": 0.90, "confidence": 0.95},
                    },
                    "tools": {
                        "mobile development": {"score_ratio": 0.85, "confidence": 0.90},
                    },
                    "infer_all": ["flutter", "mobile development"],
                },
            }
        },
        
        "scala": {
            "domain_skills": {
                "big_data": {
                    "min_scala_score": 70,
                    "frameworks": {
                        "spark": {"score_ratio": 0.80, "confidence": 0.85},
                        "akka": {"score_ratio": 0.70, "confidence": 0.75},
                    },
                    "tools": {
                        "big data": {"score_ratio": 0.75, "confidence": 0.80},
                        "data engineering": {"score_ratio": 0.65, "confidence": 0.70},
                    },
                    "infer_all": ["spark"],
                },
            }
        },
        
        "c++": {
            "domain_skills": {
                "systems_programming": {
                    "min_c++_score": 70,
                    "tools": {
                        "systems programming": {"score_ratio": 0.80, "confidence": 0.85},
                        "memory management": {"score_ratio": 0.70, "confidence": 0.75},
                    },
                    "infer_all": ["systems programming"],
                },
                "game_development": {
                    "min_c++_score": 70,
                    "frameworks": {
                        "unreal engine": {"score_ratio": 0.85, "confidence": 0.90},
                    },
                    "tools": {
                        "game development": {"score_ratio": 0.80, "confidence": 0.85},
                        "3d graphics": {"score_ratio": 0.65, "confidence": 0.70},
                    },
                    "infer_all": ["unreal engine", "game development"],
                },
            }
        },
        
        "sql": {
            "domain_skills": {
                "database": {
                    "min_sql_score": 70,
                    "tools": {
                        "database": {"score_ratio": 0.85, "confidence": 0.90},
                        "postgresql": {"score_ratio": 0.70, "confidence": 0.80},
                        "mysql": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "infer_all": ["database"],
                    "infer_one_of": ["postgresql", "mysql"],
                },
                "data_analysis": {
                    "min_sql_score": 70,
                    "tools": {
                        "database": {"score_ratio": 0.80, "confidence": 0.85},
                        "data analysis": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "infer_all": ["database", "data analysis"],
                },
            }
        },
        
        "docker": {
            "domain_skills": {
                "devops": {
                    "min_docker_score": 70,
                    "tools": {
                        "containerization": {"score_ratio": 0.85, "confidence": 0.90},
                        "devops": {"score_ratio": 0.75, "confidence": 0.85},
                        "ci/cd": {"score_ratio": 0.60, "confidence": 0.75},
                    },
                    "frameworks": {
                        "kubernetes": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "infer_all": ["containerization", "devops"],
                },
            }
        },
        
        "kubernetes": {
            "domain_skills": {
                "devops": {
                    "min_kubernetes_score": 70,
                    "tools": {
                        "docker": {"score_ratio": 0.85, "confidence": 0.90},
                        "devops": {"score_ratio": 0.80, "confidence": 0.85},
                        "cloud": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "frameworks": {
                        "helm": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_all": ["docker", "devops"],
                },
            }
        },
        
        "aws": {
            "domain_skills": {
                "cloud": {
                    "min_aws_score": 70,
                    "tools": {
                        "cloud": {"score_ratio": 0.85, "confidence": 0.90},
                        "ec2": {"score_ratio": 0.70, "confidence": 0.80},
                        "s3": {"score_ratio": 0.70, "confidence": 0.80},
                        "lambda": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "infer_all": ["cloud"],
                },
                "devops": {
                    "min_aws_score": 70,
                    "tools": {
                        "cloud": {"score_ratio": 0.80, "confidence": 0.85},
                        "devops": {"score_ratio": 0.75, "confidence": 0.80},
                        "infrastructure as code": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "frameworks": {
                        "terraform": {"score_ratio": 0.70, "confidence": 0.80},
                    },
                    "infer_all": ["cloud", "devops"],
                },
            }
        },
        
        "react": {
            "domain_skills": {
                "frontend": {
                    "min_react_score": 70,
                    "tools": {
                        "javascript": {"score_ratio": 0.85, "confidence": 0.90},
                        "html": {"score_ratio": 0.70, "confidence": 0.85},
                        "css": {"score_ratio": 0.70, "confidence": 0.85},
                    },
                    "frameworks": {
                        "next.js": {"score_ratio": 0.65, "confidence": 0.75},
                    },
                    "libraries": {
                        "redux": {"score_ratio": 0.60, "confidence": 0.70},
                    },
                    "infer_all": ["javascript", "html", "css"],
                },
                "mobile": {
                    "min_react_score": 75,
                    "frameworks": {
                        "react native": {"score_ratio": 0.80, "confidence": 0.85},
                    },
                    "tools": {
                        "mobile development": {"score_ratio": 0.75, "confidence": 0.80},
                    },
                    "infer_all": ["react native", "mobile development"],
                },
            }
        },
    }
    
    def __init__(self, config_path: Optional[str] = None, 
                 enable_inference: bool = True,
                 enable_reverse_inference: bool = True,
                 min_confidence: float = 0.70,
                 include_presence_features: bool = True,
                 include_domain_features: bool = True):
        """
        Initialize Enhanced Feature Engineer.
        
        Args:
            config_path: Optional path to custom configuration file
            enable_inference: Enable smart skill inference
            enable_reverse_inference: Enable domain-aware reverse inference
            min_confidence: Minimum confidence threshold for reverse inference (0.0-1.0)
            include_presence_features: Add binary presence flags to distinguish 0 (weak) vs missing
            include_domain_features: Add domain-level aggregated features
        """
        super().__init__(config_path)
        self.enable_inference = enable_inference
        self.enable_reverse_inference = enable_reverse_inference
        self.min_confidence = min_confidence
        self.include_presence_features = include_presence_features
        self.include_domain_features = include_domain_features
        
    def extract_features(
        self,
        candidate_id: str,
        normalized_skills: List[str],
        normalized_scores: Dict[str, float],
        include_metadata: bool = False,
        return_array: bool = False
    ) -> Dict:
        """
        Extract features with smart skill inference and enhanced encoding.
        
        Overrides parent method to add:
        1. Skill inference (forward + reverse)
        2. Presence flags to distinguish 0 (weak) vs missing
        3. Domain-level features
        
        Args:
            return_array: If True, also return features as numpy array and feature names list
        """
        # Apply skill inference if enabled
        if self.enable_inference:
            enhanced_scores, inference_metadata = self._apply_skill_inference(
                normalized_skills,
                normalized_scores
            )
        else:
            enhanced_scores = normalized_scores.copy()
            inference_metadata = {}
        
        # Store original presence before adding inferred skills
        original_skills = set(normalized_scores.keys())
        
        # Extract features using parent class (with enhanced scores)
        features_dict = super().extract_features(
            candidate_id=candidate_id,
            normalized_skills=list(enhanced_scores.keys()),
            normalized_scores=enhanced_scores,
            include_metadata=include_metadata
        )
        
        # Add enhanced encoding features to solve "too many zeros" problem
        if self.include_presence_features or self.include_domain_features:
            enhanced_features = self._add_enhanced_features_dict(
                features_dict['features'],
                enhanced_scores,
                original_skills,
                inference_metadata
            )
            
            # Update features
            features_dict['features'].update(enhanced_features)
            features_dict['feature_count'] = len(features_dict['features'])
        
        # Add inference metadata
        if include_metadata and inference_metadata:
            if 'metadata' not in features_dict:
                features_dict['metadata'] = {}
            features_dict['metadata']['skill_inference'] = inference_metadata
        
        # Optionally convert to array format
        if return_array:
            feature_names = sorted(features_dict['features'].keys())
            feature_vector = np.array([features_dict['features'][name] for name in feature_names])
            features_dict['vector'] = feature_vector
            features_dict['feature_names'] = feature_names
        
        return features_dict
    
    def _apply_skill_inference(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Tuple[Dict[str, float], Dict]:
        """
        Apply forward + reverse skill inference with confidence tracking.
        
        Returns:
            - enhanced_scores: Original + inferred skills
            - metadata: Inference details
        """
        enhanced_scores = scores.copy()
        metadata = {
            'forward_inferences': [],
            'reverse_inferences': [],
            'confidence_filtered': []
        }
        
        # Step 1: Forward inference (Specific → General) - HIGH confidence
        for skill, score in scores.items():
            if skill in self.SKILL_DEPENDENCIES:
                dependencies = self.SKILL_DEPENDENCIES[skill]
                
                for dep_skill, multiplier in dependencies.items():
                    # Only infer if skill is NOT already present
                    if dep_skill not in enhanced_scores or enhanced_scores[dep_skill] == 0:
                        inferred_score = score * multiplier
                        enhanced_scores[dep_skill] = inferred_score
                        
                        # Track metadata
                        metadata['forward_inferences'].append({
                            'skill': dep_skill,
                            'score': inferred_score,
                            'source': skill,
                            'confidence': multiplier,
                            'type': 'forward_strong'
                        })
        
        # Step 2: Domain-aware reverse inference (General → Specific) - MEDIUM confidence
        if self.enable_reverse_inference:
            # Detect domain
            domain = self._detect_candidate_domain(enhanced_scores)
            
            # Apply reverse inference with domain context
            reverse_scores, reverse_meta = self._apply_reverse_inference(
                enhanced_scores,
                domain
            )
            
            # Merge reverse inferences
            for skill, score in reverse_scores.items():
                if skill not in enhanced_scores or enhanced_scores[skill] == 0:
                    enhanced_scores[skill] = score
            
            metadata['reverse_inferences'] = reverse_meta
            metadata['detected_domain'] = domain
        
        return enhanced_scores, metadata
    
    def _apply_reverse_inference(
        self,
        candidate_scores: Dict[str, float],
        candidate_domain: str
    ) -> Tuple[Dict[str, float], List[Dict]]:
        """
        Apply context-aware reverse inference (General → Specific).
        
        Args:
            candidate_scores: Current scores (including forward inferences)
            candidate_domain: Detected primary domain
            
        Returns:
            - inferred_scores: Skills to add
            - metadata: Inference details
        """
        inferred_scores = {}
        metadata = []
        
        for base_skill, config in self.SKILL_REVERSE_INFERENCE.items():
            # Check if base skill is present
            if base_skill not in candidate_scores:
                continue
            
            base_score = candidate_scores[base_skill]
            domain_skills = config.get("domain_skills", {})
            
            # Check if we have config for this domain
            if candidate_domain not in domain_skills:
                continue
            
            domain_config = domain_skills[candidate_domain]
            
            # Check minimum score threshold
            min_score_key = f"min_{base_skill.replace(' ', '_')}_score"
            min_score = domain_config.get(min_score_key, 70)
            
            if base_score < min_score:
                continue
            
            # Get skills to infer
            frameworks = domain_config.get("frameworks", {})
            libraries = domain_config.get("libraries", {})
            tools = domain_config.get("tools", {})
            databases = domain_config.get("databases", {})
            
            all_inferrable = {**frameworks, **libraries, **tools, **databases}
            
            # Check if candidate already mentioned any of these skills
            mentioned_skills = [
                skill for skill in all_inferrable.keys()
                if skill in candidate_scores and candidate_scores[skill] > 0
            ]
            
            # Strategy 1: infer_all - Always infer these skills (core dependencies)
            infer_all = domain_config.get("infer_all", [])
            
            for skill in infer_all:
                if skill in mentioned_skills:
                    continue  # Already has this skill
                
                if skill in all_inferrable:
                    skill_config = all_inferrable[skill]
                    inferred_score = base_score * skill_config["score_ratio"]
                    confidence = skill_config["confidence"]
                    
                    # Apply confidence filter
                    if confidence >= self.min_confidence:
                        inferred_scores[skill] = inferred_score
                        
                        metadata.append({
                            'skill': skill,
                            'score': inferred_score,
                            'source': base_skill,
                            'confidence': confidence,
                            'type': 'domain_required',
                            'domain': candidate_domain
                        })
            
            # Strategy 2: infer_one_of - Infer ONE skill if none are mentioned
            infer_one_of = domain_config.get("infer_one_of", [])
            
            if infer_one_of:
                # Filter out skills in infer_all (already handled)
                candidates = [s for s in infer_one_of if s not in infer_all]
                
                # Check if ANY of these skills are already mentioned
                has_any = any(skill in mentioned_skills for skill in candidates)
                
                if not has_any and candidates:
                    # Pick the one with highest confidence
                    valid_candidates = [s for s in candidates if s in all_inferrable]
                    
                    if valid_candidates:
                        best_skill = max(
                            valid_candidates,
                            key=lambda s: all_inferrable[s].get("confidence", 0)
                        )
                        
                        skill_config = all_inferrable[best_skill]
                        inferred_score = base_score * skill_config["score_ratio"]
                        confidence = skill_config["confidence"]
                        
                        # Apply confidence filter
                        if confidence >= self.min_confidence:
                            inferred_scores[best_skill] = inferred_score
                            
                            metadata.append({
                                'skill': best_skill,
                                'score': inferred_score,
                                'source': base_skill,
                                'confidence': confidence,
                                'type': 'domain_likely',
                                'domain': candidate_domain
                            })
        
        return inferred_scores, metadata
    
    def _detect_candidate_domain(self, candidate_scores: Dict[str, float]) -> str:
        """
        Detect primary domain from candidate scores with granularity.
        
        Returns domain name: 'machine_learning', 'backend', 'frontend', etc.
        """
        DOMAIN_GROUPS = {
            # Specific first (more precise)
            'machine_learning': [
                'machine learning', 'deep learning', 'neural network',
                'tensorflow', 'pytorch', 'model training', 'nlp', 
                'computer vision', 'scikit-learn', 'sklearn'
            ],
            'data_science': [
                'data science', 'data analysis', 'statistics', 'pandas', 'numpy',
                'data visualization', 'jupyter', 'r programming', 'tableau'
            ],
            'frontend': [
                'react', 'vue', 'angular', 'javascript', 'typescript', 
                'css', 'html', 'frontend', 'ui/ux', 'next.js', 'svelte'
            ],
            'backend': [
                'python', 'java', 'go', 'node.js', 'spring', 'django', 'fastapi',
                'backend', 'api', 'microservices', 'rest api', 'graphql', 'express'
            ],
            'devops': [
                'docker', 'kubernetes', 'jenkins', 'terraform', 'ansible',
                'ci/cd', 'devops', 'infrastructure', 'monitoring'
            ],
            'mobile': [
                'react native', 'flutter', 'swift', 'kotlin', 'ios', 'android',
                'mobile development'
            ],
            'full-stack': [
                'full-stack', 'full stack development'
            ]
        }
        
        domain_scores = {}
        for domain, skills in DOMAIN_GROUPS.items():
            scores = [candidate_scores.get(skill, 0) for skill in skills]
            valid_scores = [s for s in scores if s > 0]
            
            if valid_scores:
                # Weighted: average score × number of skills
                domain_scores[domain] = np.mean(valid_scores) * len(valid_scores)
            else:
                domain_scores[domain] = 0
        
        # Get top domain
        if max(domain_scores.values()) > 0:
            primary_domain = max(domain_scores, key=domain_scores.get)
            
            # Special case: If has ML but also has data science, prefer ML
            if primary_domain == 'data_science' and domain_scores.get('machine_learning', 0) > domain_scores['data_science'] * 0.7:
                return 'machine_learning'
            
            return primary_domain
        
        return 'backend'  # Default fallback
    
    def _add_enhanced_features_dict(
        self,
        base_features: Dict[str, float],
        enhanced_scores: Dict[str, float],
        original_skills: Set[str],
        inference_metadata: Dict
    ) -> Dict[str, float]:
        """
        Add enhanced features to solve "too many zeros" problem.
        
        Adds:
        1. Binary presence flags: distinguish "0 (weak)" from "missing"
        2. Inference flags: distinguish "original" from "inferred"
        3. Domain-level features: aggregate scores by domain
        
        Args:
            base_features: Features from parent class (dict format)
            enhanced_scores: Scores after inference
            original_skills: Skills explicitly mentioned (before inference)
            inference_metadata: Metadata about inferences
            
        Returns:
            Dictionary of additional features to add
        """
        additional_features = {}
        
        # Solution to "too many zeros" problem:
        # Problem: value 0 is ambiguous - could mean "weak skill" or "not mentioned"
        # Solution: Add binary flags to distinguish these cases
        
        if self.include_presence_features:
            # Add binary presence flags for skills in enhanced_scores
            # This allows clustering to distinguish:
            # - skill_score=0, has_skill=1 → candidate HAS skill but it's weak
            # - skill_score=0, has_skill=0 → candidate DOESN'T have skill (missing)
            
            for skill in list(enhanced_scores.keys())[:50]:  # Top 50 to avoid explosion
                # Check if this skill was originally mentioned (not inferred)
                is_original = skill in original_skills
                
                additional_features[f"has_{skill.replace(' ', '_')}"] = 1.0 if is_original else 0.0
        
        if self.include_domain_features:
            # Add domain-level aggregated features
            # This helps clustering identify specialists vs generalists
            
            DOMAIN_KEYWORDS = {
                'frontend': ['react', 'vue', 'angular', 'javascript', 'typescript', 'css', 'html', 'frontend', 'ui', 'next.js', 'svelte'],
                'backend': ['python', 'java', 'go', 'node', 'django', 'fastapi', 'spring', 'backend', 'api', 'rest', 'graphql', 'express', 'flask'],
                'data': ['pandas', 'numpy', 'tensorflow', 'pytorch', 'machine learning', 'data', 'ml', 'scikit', 'keras', 'deep learning'],
                'devops': ['docker', 'kubernetes', 'jenkins', 'terraform', 'ci/cd', 'devops', 'deployment', 'ansible', 'cloud', 'aws', 'azure', 'gcp'],
                'mobile': ['react native', 'flutter', 'swift', 'kotlin', 'ios', 'android', 'mobile'],
                'database': ['sql', 'postgresql', 'mysql', 'mongodb', 'database', 'nosql', 'sqlite', 'sqlalchemy', 'redis', 'dynamodb'],
            }
            
            for domain, keywords in DOMAIN_KEYWORDS.items():
                # Calculate domain presence (how many skills in this domain)
                domain_skills = [
                    skill for skill in enhanced_scores.keys()
                    if any(kw in skill.lower() for kw in keywords)
                ]
                
                domain_presence = min(len(domain_skills) / 5.0, 1.0)  # Normalize by 5 skills
                additional_features[f"domain_{domain}_presence"] = domain_presence
                
                # Calculate domain strength (average score in this domain)
                if domain_skills:
                    domain_scores = [enhanced_scores[skill] for skill in domain_skills]
                    domain_strength = np.mean(domain_scores) / 100.0  # Normalize to 0-1
                else:
                    domain_strength = 0.0
                
                additional_features[f"domain_{domain}_strength"] = domain_strength
        
        # Add inference statistics as meta-features
        if inference_metadata:
            forward_count = len(inference_metadata.get('forward_inferences', []))
            reverse_count = len(inference_metadata.get('reverse_inferences', []))
            
            additional_features['inference_forward_count'] = forward_count / 10.0  # Normalize
            additional_features['inference_reverse_count'] = reverse_count / 10.0  # Normalize
            
            # Average confidence of inferences
            all_inferences = (
                inference_metadata.get('forward_inferences', []) +
                inference_metadata.get('reverse_inferences', [])
            )
            
            if all_inferences:
                avg_confidence = np.mean([inf.get('confidence', 0) for inf in all_inferences])
            else:
                avg_confidence = 0.0
            
            additional_features['inference_avg_confidence'] = avg_confidence
        
        return additional_features
    
    def get_inference_stats(self, candidates: List[Dict]) -> Dict:
        """
        Get statistics about skill inference across all candidates.
        
        Useful for understanding impact of inference.
        """
        stats = {
            'total_candidates': len(candidates),
            'candidates_with_forward_inference': 0,
            'candidates_with_reverse_inference': 0,
            'total_forward_inferences': 0,
            'total_reverse_inferences': 0,
            'avg_skills_before': 0,
            'avg_skills_after': 0,
            'domain_distribution': defaultdict(int),
            'most_inferred_skills': defaultdict(int),
        }
        
        skills_before_list = []
        skills_after_list = []
        
        for candidate in candidates:
            original_scores = candidate.get('normalized_scores', {})
            skills_before = len([s for s, v in original_scores.items() if v > 0])
            skills_before_list.append(skills_before)
            
            # Apply inference
            enhanced_scores, metadata = self._apply_skill_inference(
                candidate.get('normalized_skills', []),
                original_scores
            )
            
            skills_after = len([s for s, v in enhanced_scores.items() if v > 0])
            skills_after_list.append(skills_after)
            
            # Track stats
            if metadata.get('forward_inferences'):
                stats['candidates_with_forward_inference'] += 1
                stats['total_forward_inferences'] += len(metadata['forward_inferences'])
                
                for inf in metadata['forward_inferences']:
                    stats['most_inferred_skills'][inf['skill']] += 1
            
            if metadata.get('reverse_inferences'):
                stats['candidates_with_reverse_inference'] += 1
                stats['total_reverse_inferences'] += len(metadata['reverse_inferences'])
                
                for inf in metadata['reverse_inferences']:
                    stats['most_inferred_skills'][inf['skill']] += 1
            
            # Track domains
            domain = metadata.get('detected_domain', 'unknown')
            stats['domain_distribution'][domain] += 1
        
        stats['avg_skills_before'] = np.mean(skills_before_list)
        stats['avg_skills_after'] = np.mean(skills_after_list)
        stats['avg_skills_added'] = stats['avg_skills_after'] - stats['avg_skills_before']
        stats['skill_increase_pct'] = (stats['avg_skills_added'] / stats['avg_skills_before'] * 100) if stats['avg_skills_before'] > 0 else 0
        
        # Sort most inferred skills
        stats['most_inferred_skills'] = dict(
            sorted(stats['most_inferred_skills'].items(), 
                   key=lambda x: x[1], 
                   reverse=True)[:20]
        )
        
        return stats
