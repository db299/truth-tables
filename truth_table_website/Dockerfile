# Use the official Python image as a base image
FROM python:3.11-slim

# Set environment variables for proper container behavior
ENV PYTHONUNBUFFERED=1 \
    FLASK_APP=server.py \
    FLASK_RUN_HOST=0.0.0.0

# Set the working directory in the container
WORKDIR /app

# Copy and install dependencies
COPY requirements.txt .

# Install any needed packages specified in requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application code into the container
COPY . .

# Expose the port Flask will run on
EXPOSE 5000

# Run the Flask application
CMD ["python3", "server.py"]