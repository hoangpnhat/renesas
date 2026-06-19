# Step 1: Build the application
FROM node:18-alpine AS build

# Set the working directory in the container
WORKDIR /app

# Copy the package.json and package-lock.json
COPY package*.json ./

# Install dependencies
RUN npm install

# Copy the rest of the project files
COPY . .

# Build the application
RUN npm run build -- --mode staging

# Step 2: Serve the application
# Expose the port the app will run on
EXPOSE 3000

# Start the server using the Node.js file
CMD ["npm", "run", "start"]
