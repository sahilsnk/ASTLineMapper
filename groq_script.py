# groq_script.py
import sys
import json
import base64
from groq import Groq # type: ignore

# Read the Base64 encoded prompt string from command line argument
if len(sys.argv) < 2:
    print("[]") # Return an empty JSON array if no input
    sys.exit(1)

encoded_prompt_str = sys.argv[1]

try:
    # Decode the entire prompt string
    prompt_content = base64.b64decode(encoded_prompt_str).decode('utf-8')
    #print(f"DEBUG(py): Decoded Full Prompt String (for Groq API):\n{prompt_content}", file=sys.stderr)
except Exception as e:
    print(json.dumps([{"error": f"Base64 decode failed in Python: {e}"}]), file=sys.stderr)
    print("[]")
    sys.exit(1)

client = Groq(api_key="GROQ_API_KEY") # Ensure API key is correct

#print(f"DEBUG(py): Prompt prepared for Groq API call. Length: {len(prompt_content)} characters.", file=sys.stderr)


response = None
try:
    response = client.chat.completions.create(
        model="llama3-70b-8192", # Or another suitable Groq model
        messages=[
            {"role": "system", "content": "You are a helpful assistant that explains C++ AST nodes in JSON format. ONLY output the JSON array, nothing else. Do not include any introductory or concluding text."}, # Strengthen system message
            {"role": "user", "content": prompt_content}
        ],
        temperature=0.0,
        # --- ADD THIS LINE ---
        response_format={"type": "json_object"} # Tells the API to expect JSON
        # --- END ADDITION ---
    )
except Exception as e:
    #print(f"DEBUG(py): Groq API call failed: {e}", file=sys.stderr)
    print(json.dumps([{"error": f"Groq API call failed: {e}"}]))
    sys.exit(1)


# Extract and print the content, which should be the JSON string from Groq
try:
    groq_response_content = response.choices[0].message.content.strip()
    #print(f"DEBUG(py): Raw Groq Response Content (from API):\n{groq_response_content}", file=sys.stderr)

    # Attempt to parse and re-dump to ensure it's valid JSON before printing
    parsed_response = json.loads(groq_response_content) # This should now succeed if Groq outputs only JSON
    # Print the clean, valid JSON to stdout for the C++ program to capture
    print(json.dumps(parsed_response, indent=2))
except json.JSONDecodeError as e:
    #print(f"DEBUG(py): Groq returned invalid JSON (Python side parse error): {groq_response_content}. Error: {e}", file=sys.stderr)
    print(json.dumps([{"error": f"Groq returned invalid JSON. Raw: '{groq_response_content}'. Error: {e}"}]))
except Exception as e:
    #print(f"DEBUG(py): Unexpected error processing Groq response in Python: {e}", file=sys.stderr)
    print(json.dumps([{"error": f"Unexpected error processing Groq response in Python: {e}"}]))
