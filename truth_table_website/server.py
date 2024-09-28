from flask import Flask, request, render_template, jsonify, session
import subprocess
import os
import glob
import shutil  
import atexit  

app = Flask(__name__)

CACHE_DIR = 'cache_files'
os.makedirs(CACHE_DIR, exist_ok=True)

# Function to get the file path based on the session id
def get_cache_file_path(session_id):
    # Generate the cache file name based on the session id
    return os.path.join(CACHE_DIR, f'{session_id}.cache')


# Function to delete the entire cache dir on unload
def cleanup_cache():
    if os.path.exists(CACHE_DIR):
        shutil.rmtree(CACHE_DIR)  # Deletes the entire cache_files directory

atexit.register(cleanup_cache)
# Function to clear the cache, received as request from the website
@app.route('/clear_cache', methods=['POST'])
def clear_cache():
    session_id = request.args.get('session_id')

    if not session_id:
        return jsonify({'error': 'No session ID provided'}), 400
    
    # Find all cache files for the given session_id
    # Given no bugs, should only be one
    # Cache files should consist of the session id followed by cache
    cache_files = glob.glob(os.path.join(CACHE_DIR, f'{session_id}*.cache'))

    if not cache_files:
        return jsonify({'error': 'No cache files found for the provided session ID'}), 404
    
    # Delete all found cache files
    for cache_file in cache_files:
        try:
            os.remove(cache_file)
        except OSError as e:
            return jsonify({'error': f'Error deleting cache file: {str(e)}'}), 500

    return jsonify({'message': 'Cache cleared successfully'}), 200

# Function to render the webpage whenever a request occurs
@app.route('/', methods=['GET', 'POST'])
def index():
    return render_template('index.html')

# Function to generate the rows and pass them back to the website
@app.route('/generate', methods=['GET', 'POST'])
def generate_rows():
    expression = request.args.get('expression')
    start = int(request.args.get('start', 0))
    end = int(request.args.get('end', 100))
    mode = request.args.get('mode', 'truth_table')  # Default to 'truth_table'
    session_id = request.args.get('session_id')
    cache_file_path = get_cache_file_path(session_id)

    
    if mode == 'true_rows':
        # This will be the case when calls are made from scrolling.
        # Simply read until we get 100 1 lines from the file
        if os.path.exists(cache_file_path):
            # Reading from existing cache file
            extracted_lines = []
            current_line_number = 0

            with open(cache_file_path, 'r') as file:
                for line in file:
                    if current_line_number < start:
                        current_line_number += 1
                        continue

                    if len(extracted_lines) >= 100:
                        break
                    current_line_number += 1
                    extracted_lines.append(line)

            return jsonify(extracted_lines)

        else:
            # Run the command and create the cache file
            command = ['./website_binary_ttable', expression, cache_file_path]
            try:
                # Run truth table to write to the cache file 
                subprocess.run(command, text=True)
                extracted_lines = []
                current_line_number = 0
                with open(cache_file_path, 'r') as file:
                    for line in file:
                       
                        extracted_lines.append(line)
                        current_line_number += 1
                        if current_line_number == 100:
                            break
                return jsonify(extracted_lines)

            except subprocess.CalledProcessError as e:
                return jsonify({'error': str(e)}), 500
            except Exception as e:
                return jsonify({'error': str(e)}), 500
        

    elif mode == 'truth_table':
        command = ['./website_binary_ttable', expression, str(start), str(end)]
        try:
            # When I only need to generate a segment, no need for caching.
            # Just capture the output from stdout
            result = subprocess.run(command, capture_output=True, text=True)
            ttable_output = result.stdout.splitlines()
            return jsonify(ttable_output)
        except subprocess.CalledProcessError as e:
            return jsonify({'error': str(e)}), 500
        except Exception as e:
            return jsonify({'error': str(e)}), 500

if __name__ == '__main__':

    app.run(host="0.0.0.0", port=5000)