# Use the lightweight stable Alpine variant of NGINX
FROM nginx:alpine

RUN rm /etc/nginx/conf.d/default.conf

COPY ./nginx/nginx.conf /etc/nginx/conf.d/my_app.conf
COPY ./dist /usr/share/nginx/html

EXPOSE 80

# Run NGINX in the foreground so the container doesn't exit immediately
CMD ["nginx", "-g", "daemon off;"]