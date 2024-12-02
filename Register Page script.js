const formOpenBtn = document.querySelector("#form-open"),
home = document.querySelector(".home"),
form_container = document.querySelector(".form_container"),
formCloseBtn = document.querySelector(".form_close"),
signupBtn = document.querySelector("#signup"),
loginBtn = document.querySelector("#login"),
pwShowHide = document.querySelectorAll(".pw_hide");

formOpenBtn.addEventListener("click", () => home.classList.add("show"))
formCloseBtn.addEventListener("click", () => home.classList.remove("show"))

pwShowHide.forEach((icon) =>
{
    icon.addEventListener("click", () =>{
        let getPwInput = icon.parentElement.querySelector("input");
        if (getPwInput.type === "password")
        {
            getPwInput.type = "text";
            icon.classList.replace("uil-eye-slash", "uil-eye");
        }
        else
        {
            getPwInput.type = "password";
            icon.classList.replace("uil-eye", "uil-eye-slash");
        }
    });
});

signupBtn.addEventListener("click", (e) => 
{
    e.preventDefault();
    form_container.classList.add("active");
});

loginBtn.addEventListener("click", (e) => 
{
    e.preventDefault();
    form_container.classList.remove("active");
});

//================================================================================

document.getElementById("login_form").addEventListener("submit", function(event) 
{
    event.preventDefault();
    var password = document.getElementById("password").value;
    var confirmPassword = document.getElementById("confirmPassword").value;
    var errorMessage = document.getElementById("error_message");

    if (password !== confirmPassword) 
    {
        errorMessage.textContent = "Passwords do not match";
        return;
    } 
    else 
    {
        errorMessage.textContent = "";
    }
});    

//===============================================================================

let emailInput = document.getElementById("sigiin_email"); 
let passwordInput = document.getElementById("sigiin_pass");
function signIn() 
{
    let email = emailInput.value; 
    let password = passwordInput.value;

    let data = 
    {
        email: email, 
        password: password
    };

fetch('https://3dd8-41-236-219-177.ngrok-free.app/signin', 
{
    method: 'POST',
    headers: 
    {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
})
.then(response => {
    if (!response.ok) 
    {
        throw new Error('Network response was not ok');
    }
    return response.json();
})
.then(data => 
{
    console.log('Response:', data);
})
.catch(error => 
{
    console.error('There was a problem with the fetch operation:', error);
});
}

document.querySelector('.form form').addEventListener('submit', function(event) 
{
event.preventDefault(); 
  signIn(); // Call 
});